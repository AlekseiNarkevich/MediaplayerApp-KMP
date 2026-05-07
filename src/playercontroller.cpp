/**
 * @file playercontroller.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Реализация контроллера воспроизведения.
 */

#include "playercontroller.h"

#include <QFileInfo>
#include <QDir>
#include <QMediaMetaData>
#include <QStandardPaths>
#include <QUrl>

PlayerController::PlayerController(QObject *parent)
    : QObject(parent)
{
    // Базовая громкость по умолчанию; значение можно менять из UI при необходимости.
    m_audioOutput.setVolume(0.8f);
    m_player.setAudioOutput(&m_audioOutput);

    connect(&m_player, &QMediaPlayer::positionChanged, this, &PlayerController::positionChanged);
    connect(&m_player, &QMediaPlayer::durationChanged, this, [this]() {
        emit durationChanged();
        applyPendingSeekIfPossible();
    });
    connect(&m_player, &QMediaPlayer::playbackStateChanged, this, &PlayerController::playingChanged);
    connect(&m_player, &QMediaPlayer::seekableChanged, this, [this]() {
        emit seekableChanged();
        applyPendingSeekIfPossible();
    });
    // После обновления метаданных пытаемся подтянуть новую обложку текущего трека.
    connect(&m_player, &QMediaPlayer::metaDataChanged, this, [this]() {
        const QString resolved = resolveCoverForTrack(m_currentTrack);
        if (resolved != m_currentCover) {
            m_currentCover = resolved;
            emit currentCoverChanged();
        }
    });
    // Ошибки воспроизведения выводим в консоль для отладки и демонстрации обработки исключительных ситуаций.
    connect(&m_player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error error, const QString &errorString) {
        Q_UNUSED(error)
        qWarning() << "PlayerController: ошибка воспроизведения:" << errorString;
    });

    if (!m_repository.initialize()) {
        qWarning() << "PlayerController: инициализация БД завершилась с ошибкой";
    }
}

QStringList PlayerController::playlist() const
{
    return m_playlist;
}

QString PlayerController::currentTrack() const
{
    return m_currentTrack;
}

bool PlayerController::playing() const
{
    return m_player.playbackState() == QMediaPlayer::PlayingState;
}

bool PlayerController::seekable() const
{
    return m_player.isSeekable();
}

qint64 PlayerController::position() const
{
    return m_player.position();
}

qint64 PlayerController::duration() const
{
    return m_player.duration();
}

QString PlayerController::currentCover() const
{
    return m_currentCover;
}

QString PlayerController::trackName(const QString &filePath) const
{
    // Если для URL задано пользовательское имя (Web), используем его в первую очередь.
    const auto it = m_displayNames.constFind(filePath);
    if (it != m_displayNames.constEnd() && !it.value().isEmpty()) {
        return it.value();
    }

    const QUrl possibleUrl(filePath);
    if (possibleUrl.isValid() && !possibleUrl.scheme().isEmpty()) {
        const QString fileNameFromUrl = QFileInfo(possibleUrl.path()).completeBaseName();
        if (!fileNameFromUrl.isEmpty()) {
            return fileNameFromUrl;
        }
        const QString fallbackName = possibleUrl.fileName();
        if (!fallbackName.isEmpty()) {
            return fallbackName;
        }
    }
    return QFileInfo(filePath).completeBaseName();
}

void PlayerController::setTrackDisplayName(const QString &filePath, const QString &displayName)
{
    if (filePath.isEmpty() || displayName.isEmpty()) {
        return;
    }
    m_displayNames.insert(filePath, displayName);
}

void PlayerController::addToPlaylist(const QString &filePath)
{
    // Не добавляем пустые элементы и дубликаты.
    if (filePath.isEmpty() || m_playlist.contains(filePath)) {
        return;
    }

    m_playlist.append(filePath);
    emit playlistChanged();

    if (!m_repository.addTrack(filePath)) {
        qWarning() << "PlayerController: не удалось сохранить трек в БД:" << filePath;
    }

    if (m_currentTrack.isEmpty()) {
        setCurrentTrackInternal(filePath);
    }
}

void PlayerController::removeFromPlaylist(const QString &filePath)
{
    const int index = m_playlist.indexOf(filePath);
    if (index < 0) {
        return;
    }

    m_playlist.removeAt(index);
    emit playlistChanged();

    if (!m_repository.removeTrack(filePath)) {
        qWarning() << "PlayerController: не удалось удалить трек из БД:" << filePath;
    }

    // Если удалили текущий трек, останавливаемся и выбираем следующий доступный.
    if (filePath == m_currentTrack) {
        stop();
        if (!m_playlist.isEmpty()) {
            setCurrentTrackInternal(m_playlist.first());
        } else {
            setCurrentTrackInternal(QString());
        }
    }
}

void PlayerController::restorePlaylist()
{
    // Восстанавливаем порядок треков из БД при запуске приложения.
    m_playlist = m_repository.loadPlaylist();
    emit playlistChanged();
    if (!m_playlist.isEmpty()) {
        setCurrentTrackInternal(m_playlist.first());
    }
}

void PlayerController::playTrack(const QString &filePath)
{
    if (filePath.isEmpty()) {
        qWarning() << "PlayerController: пустой путь к треку";
        return;
    }

    setCurrentTrackInternal(filePath);
    const QString resolvedCover = resolveCoverForTrack(filePath);
    if (resolvedCover != m_currentCover) {
        m_currentCover = resolvedCover;
        emit currentCoverChanged();
    }

    // Преобразуем вход в URL: поддерживаем локальные пути и content:// URI на Android.
    QUrl sourceUrl;
    if (filePath.startsWith("file://") ||
        filePath.startsWith("content://") ||
        filePath.startsWith("qrc:/") ||
        filePath.startsWith("http://") ||
        filePath.startsWith("https://")) {
        sourceUrl = QUrl(filePath);
    } else {
        sourceUrl = QUrl::fromLocalFile(filePath);
    }

    m_player.setSource(sourceUrl);
    m_player.play();
}

void PlayerController::play()
{
    // Если текущий трек еще не задан, стартуем с первого элемента плейлиста.
    if (m_currentTrack.isEmpty() && !m_playlist.isEmpty()) {
        playTrack(m_playlist.first());
        return;
    }
    m_player.play();
}

void PlayerController::pause()
{
    m_player.pause();
}

void PlayerController::stop()
{
    m_player.stop();
}

void PlayerController::seek(qint64 newPosition)
{
    if (newPosition < 0) {
        return;
    }
    if (!m_player.isSeekable() || m_player.duration() <= 0) {
        m_pendingSeek = newPosition;
        return;
    }
    m_player.setPosition(newPosition);
    m_pendingSeek = -1;
}

void PlayerController::setCurrentTrackInternal(const QString &filePath)
{
    if (m_currentTrack == filePath) {
        return;
    }
    m_currentTrack = filePath;
    emit currentTrackChanged();
}

QString PlayerController::resolveCoverForTrack(const QString &filePath) const
{
    const QString defaultCover = "qrc:/assets/default_cover.svg";
    if (filePath.isEmpty()) {
        return defaultCover;
    }

    // 1) Пытаемся взять встроенную обложку из метаданных медиафайла.
    const QImage embeddedCover = m_player.metaData().value(QMediaMetaData::ThumbnailImage).value<QImage>();
    if (!embeddedCover.isNull()) {
        const QString cachedEmbeddedPath = saveEmbeddedCover(embeddedCover);
        if (!cachedEmbeddedPath.isEmpty()) {
            return cachedEmbeddedPath;
        }
    }

    // 2) Если встроенной обложки нет, ищем стандартные имена файлов в папке трека.
    const QFileInfo info(filePath);
    const QDir dir = info.dir();
    const QStringList candidateNames = {
        info.completeBaseName() + ".jpg",
        info.completeBaseName() + ".jpeg",
        info.completeBaseName() + ".png",
        "cover.jpg",
        "cover.jpeg",
        "cover.png",
        "folder.jpg",
        "folder.jpeg",
        "folder.png",
        "front.jpg",
        "front.png"
    };

    for (const QString &candidate : candidateNames) {
        const QString absoluteCandidate = dir.absoluteFilePath(candidate);
        if (QFileInfo::exists(absoluteCandidate)) {
            return absoluteCandidate;
        }
    }

    // 3) Если ничего не найдено, возвращаем дефолтную обложку из ресурсов.
    return defaultCover;
}

QString PlayerController::saveEmbeddedCover(const QImage &image) const
{
    const QString tempDirPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempDirPath.isEmpty()) {
        return QString();
    }

    QDir tempDir(tempDirPath);
    if (!tempDir.exists() && !tempDir.mkpath(".")) {
        return QString();
    }

    const QString targetPath = tempDir.absoluteFilePath("mediaplayer_embedded_cover.png");
    if (!image.save(targetPath, "PNG")) {
        return QString();
    }
    return targetPath;
}

void PlayerController::applyPendingSeekIfPossible()
{
    if (m_pendingSeek < 0) {
        return;
    }
    if (!m_player.isSeekable() || m_player.duration() <= 0) {
        return;
    }
    m_player.setPosition(m_pendingSeek);
    m_pendingSeek = -1;
}
