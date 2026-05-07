/**
 * @file medialibrarymodel.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Реализация модели медиатеки для QML.
 */

#include "medialibrarymodel.h"

#include <QFileInfo>
#include <QDirIterator>

MediaLibraryModel::MediaLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Поддерживаемые расширения аудио для desktop-сканирования папки.
    m_filters << "*.mp3" << "*.wav" << "*.flac" << "*.ogg" << "*.m4a" << "*.aac";
}

int MediaLibraryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_tracks.count();
}

QVariant MediaLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size()) {
        return QVariant();
    }

    const TrackItem &item = m_tracks.at(index.row());
    switch (role) {
    case TitleRole:
        return item.title;
    case FilePathRole:
        return item.filePath;
    case CoverPathRole:
        return item.coverPath;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MediaLibraryModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {FilePathRole, "filePath"},
        {CoverPathRole, "coverPath"}
    };
}

QString MediaLibraryModel::currentFolder() const
{
    return m_currentFolder;
}

QString MediaLibraryModel::fileAt(int row) const
{
    if (row < 0 || row >= m_tracks.size()) {
        return QString();
    }
    return m_tracks.at(row).filePath;
}

void MediaLibraryModel::scanFolder(const QString &folderPath)
{
    const QDir dir(folderPath);
    if (!dir.exists()) {
        qWarning() << "MediaLibraryModel: папка не существует:" << folderPath;
        emit scanFinished(0);
        return;
    }

    // Полный пересчет модели: очищаем старые элементы и собираем новые.
    beginResetModel();
    m_tracks.clear();

    QDirIterator it(folderPath, m_filters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString filePath = it.next();
        QFileInfo info(filePath);

        TrackItem item;
        // В title сохраняем читаемое имя трека без расширения.
        item.title = info.completeBaseName();
        item.filePath = info.absoluteFilePath();
        item.coverPath = resolveCoverPath(info);
        m_tracks.append(item);
    }

    m_currentFolder = dir.absolutePath();
    endResetModel();

    emit currentFolderChanged();
    emit scanFinished(m_tracks.count());
}

QString MediaLibraryModel::resolveCoverPath(const QFileInfo &audioFileInfo) const
{
    const QString defaultCover = "qrc:/assets/default_cover.svg";
    const QDir dir = audioFileInfo.dir();

    const QStringList candidateNames = {
        audioFileInfo.completeBaseName() + ".jpg",
        audioFileInfo.completeBaseName() + ".jpeg",
        audioFileInfo.completeBaseName() + ".png",
        "cover.jpg",
        "cover.jpeg",
        "cover.png",
        "folder.jpg",
        "folder.jpeg",
        "folder.png",
        "front.jpg",
        "front.png"
    };

    // Ищем обложку по стандартным именам рядом с аудиофайлом.
    for (const QString &candidate : candidateNames) {
        const QString absoluteCandidate = dir.absoluteFilePath(candidate);
        if (QFileInfo::exists(absoluteCandidate)) {
            return absoluteCandidate;
        }
    }

    // Если обложка не найдена, используем заглушку.
    return defaultCover;
}
