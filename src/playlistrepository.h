/**
 * @file playlistrepository.h
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Репозиторий плейлиста на SQLite.
 */

#ifndef PLAYLISTREPOSITORY_H
#define PLAYLISTREPOSITORY_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>

/**
 * @class PlaylistRepository
 * @brief Класс-обертка для сохранения и чтения плейлиста в SQLite.
 */
class PlaylistRepository : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Создает репозиторий и настраивает подключение к SQLite.
     * @param parent Родительский QObject.
     */
    explicit PlaylistRepository(QObject *parent = nullptr);

    /**
     * @brief Закрывает и удаляет подключение к БД.
     */
    ~PlaylistRepository() override;

    /**
     * @brief Инициализирует SQLite-базу и таблицу плейлиста.
     * @return true, если инициализация успешна.
     */
    bool initialize();

    /**
     * @brief Загружает список файлов из БД.
     */
    QStringList loadPlaylist() const;

    /**
     * @brief Добавляет трек в БД (если отсутствует).
     */
    bool addTrack(const QString &filePath);

    /**
     * @brief Удаляет трек из БД.
     */
    bool removeTrack(const QString &filePath);

private:
    /**
     * @brief Возвращает имя подключения QSqlDatabase.
     */
    QString connectionName() const;

    /**
     * @brief Возвращает объект подключения к БД по имени connectionName().
     */
    QSqlDatabase database() const;

    /**
     * @brief Проверяет/открывает подключение к БД.
     * @return true, если БД доступна и открыта.
     */
    bool ensureOpened() const;
};

#endif // PLAYLISTREPOSITORY_H
