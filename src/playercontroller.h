/**
 * @file playercontroller.h
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Контроллер воспроизведения и управления плейлистом.
 */

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QImage>
#include <QHash>

#include "playlistrepository.h"

/**
 * @class PlayerController
 * @brief Управляет воспроизведением аудио и синхронизирует плейлист с SQLite.
 */
class PlayerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList playlist READ playlist NOTIFY playlistChanged)
    Q_PROPERTY(QString currentTrack READ currentTrack NOTIFY currentTrackChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString currentCover READ currentCover NOTIFY currentCoverChanged)

public:
    /**
     * @brief Создает контроллер и инициализирует мультимедиа-объекты.
     * @param parent Родительский QObject.
     */
    explicit PlayerController(QObject *parent = nullptr);

    /**
     * @brief Возвращает текущий плейлист (список путей/URL треков).
     */
    QStringList playlist() const;

    /**
     * @brief Возвращает путь/URL текущего трека.
     */
    QString currentTrack() const;

    /**
     * @brief Возвращает true, если в данный момент идет воспроизведение.
     */
    bool playing() const;

    /**
     * @brief Возвращает true, если текущий источник поддерживает перемотку.
     */
    bool seekable() const;

    /**
     * @brief Возвращает текущую позицию воспроизведения в мс.
     */
    qint64 position() const;

    /**
     * @brief Возвращает длительность текущего трека в мс.
     */
    qint64 duration() const;

    /**
     * @brief Возвращает путь к текущей обложке (файл или ресурс).
     */
    QString currentCover() const;

    /**
     * @brief Возвращает имя трека без пути (для UI).
     */
    Q_INVOKABLE QString trackName(const QString &filePath) const;

    /**
     * @brief Сохраняет человекочитаемое отображаемое имя для трека.
     * @param filePath URL/путь трека.
     * @param displayName Имя, которое нужно показывать в интерфейсе.
     */
    Q_INVOKABLE void setTrackDisplayName(const QString &filePath, const QString &displayName);

public slots:
    /**
     * @brief Добавляет трек в плейлист и БД.
     */
    void addToPlaylist(const QString &filePath);

    /**
     * @brief Удаляет трек из плейлиста и БД.
     */
    void removeFromPlaylist(const QString &filePath);

    /**
     * @brief Загружает плейлист из БД.
     */
    void restorePlaylist();

    /**
     * @brief Запускает воспроизведение выбранного трека.
     */
    void playTrack(const QString &filePath);

    /**
     * @brief Запускает или возобновляет текущее воспроизведение.
     */
    void play();

    /**
     * @brief Ставит воспроизведение на паузу.
     */
    void pause();

    /**
     * @brief Останавливает воспроизведение.
     */
    void stop();

    /**
     * @brief Выполняет перемотку к указанной позиции.
     * @param newPosition Позиция в миллисекундах.
     */
    void seek(qint64 newPosition);

signals:
    /**
     * @brief Сигнал изменения содержимого плейлиста.
     */
    void playlistChanged();

    /**
     * @brief Сигнал изменения текущего трека.
     */
    void currentTrackChanged();

    /**
     * @brief Сигнал изменения статуса воспроизведения.
     */
    void playingChanged();

    /**
     * @brief Сигнал изменения возможности перемотки.
     */
    void seekableChanged();

    /**
     * @brief Сигнал изменения позиции воспроизведения.
     */
    void positionChanged();

    /**
     * @brief Сигнал изменения длительности текущего трека.
     */
    void durationChanged();

    /**
     * @brief Сигнал изменения пути обложки текущего трека.
     */
    void currentCoverChanged();

private:
    /**
     * @brief Обновляет текущий трек и отправляет сигнал об изменении.
     * @param filePath Новый путь/URL трека.
     */
    void setCurrentTrackInternal(const QString &filePath);
    void applyPendingSeekIfPossible();

    /**
     * @brief Определяет подходящую обложку для трека.
     * @details Порядок: встроенная обложка -> файл в папке -> заглушка.
     * @param filePath Путь/URL трека.
     * @return Путь к изображению обложки.
     */
    QString resolveCoverForTrack(const QString &filePath) const;

    /**
     * @brief Сохраняет встроенную обложку во временный PNG-файл.
     * @param image Картинка из метаданных.
     * @return Абсолютный путь к сохраненному файлу или пустая строка.
     */
    QString saveEmbeddedCover(const QImage &image) const;

    /// Список треков плейлиста (локальная копия, синхронизируется с SQLite).
    QStringList m_playlist;
    /// Текущий трек (путь или URL), выбранный для воспроизведения.
    QString m_currentTrack;
    /// Текущая активная обложка (файл/ресурс), отображаемая в UI.
    QString m_currentCover = "qrc:/assets/default_cover.svg";
    /// Переопределенные отображаемые имена треков (например, для WebAssembly URL).
    QHash<QString, QString> m_displayNames;
    /// Низкоуровневый плеер QtMultimedia.
    QMediaPlayer m_player;
    /// Аудиовыход для управления громкостью и маршрутом звука.
    QAudioOutput m_audioOutput;
    /// Репозиторий плейлиста в SQLite.
    PlaylistRepository m_repository;
    /// Отложенная позиция перемотки до момента, когда источник станет seekable.
    qint64 m_pendingSeek = -1;
};

#endif // PLAYERCONTROLLER_H
