/**
 * @file medialibrarymodel.h
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Модель медиатеки: сканирование папки и выдача списка аудиофайлов в QML.
 */

#ifndef MEDIALIBRARYMODEL_H
#define MEDIALIBRARYMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

/**
 * @class MediaLibraryModel
 * @brief Модель списка аудиотреков для отображения в QML ListView.
 *
 * Класс выполняет:
 * - сканирование выбранной пользователем папки;
 * - формирование списка поддерживаемых аудиофайлов;
 * - предоставление метаданных (имя, путь, реальная или резервная обложка).
 */
class MediaLibraryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentFolder READ currentFolder NOTIFY currentFolderChanged)

public:
    /**
     * @enum Roles
     * @brief Роли модели для доступа из QML.
     */
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        FilePathRole,
        CoverPathRole
    };

    /**
     * @brief Создает модель медиатеки и инициализирует фильтры расширений.
     * @param parent Родительский QObject.
     */
    explicit MediaLibraryModel(QObject *parent = nullptr);

    /**
     * @brief Возвращает количество элементов в модели.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Возвращает данные элемента по индексу и роли.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Возвращает таблицу имен ролей для доступа из QML.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Возвращает путь к текущей просканированной папке.
     */
    QString currentFolder() const;

    /**
     * @brief Возвращает абсолютный путь трека по индексу.
     */
    Q_INVOKABLE QString fileAt(int row) const;

public slots:
    /**
     * @brief Сканирует указанную папку и обновляет список треков.
     * @param folderPath Абсолютный путь к папке с медиафайлами.
     */
    void scanFolder(const QString &folderPath);

signals:
    /**
     * @brief Сигнал об изменении текущей папки медиатеки.
     */
    void currentFolderChanged();

    /**
     * @brief Сигнал о том, что сканирование завершено.
     * @param filesCount Количество найденных аудиофайлов.
     */
    void scanFinished(int filesCount);

private:
    /**
     * @brief Пытается найти файл обложки рядом с треком.
     * @param audioFileInfo Информация о текущем аудиофайле.
     * @return Абсолютный путь к обложке или путь к заглушке из ресурсов.
     */
    QString resolveCoverPath(const QFileInfo &audioFileInfo) const;

    struct TrackItem {
        QString title;
        QString filePath;
        QString coverPath;
    };

    QList<TrackItem> m_tracks;
    QString m_currentFolder;
    QStringList m_filters;
};

#endif // MEDIALIBRARYMODEL_H
