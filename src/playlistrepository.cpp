/**
 * @file playlistrepository.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Реализация репозитория плейлиста на SQLite.
 */

#include "playlistrepository.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>
#include <QVariant>

namespace {
const char *kCreateTableSql =
    "CREATE TABLE IF NOT EXISTS playlist ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "file_path TEXT NOT NULL UNIQUE"
    ");";
}

PlaylistRepository::PlaylistRepository(QObject *parent)
    : QObject(parent)
{
    // Храним БД в каталоге данных приложения.
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir().mkpath(appData)) {
        qWarning() << "PlaylistRepository: невозможно создать каталог:" << appData;
    }

    // Регистрируем отдельное именованное подключение для репозитория.
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    db.setDatabaseName(appData + QDir::separator() + "playlist.db");
}

PlaylistRepository::~PlaylistRepository()
{
    // Корректно закрываем и удаляем именованное подключение к БД.
    const QString name = connectionName();
    if (QSqlDatabase::contains(name)) {
        QSqlDatabase::database(name).close();
        QSqlDatabase::removeDatabase(name);
    }
}

bool PlaylistRepository::initialize()
{
    // Инициализируем структуру таблицы, если она еще не создана.
    if (!ensureOpened()) {
        return false;
    }

    QSqlQuery query(database());
    if (!query.exec(QString::fromUtf8(kCreateTableSql))) {
        qWarning() << "PlaylistRepository: ошибка создания таблицы:" << query.lastError().text();
        return false;
    }
    return true;
}

QStringList PlaylistRepository::loadPlaylist() const
{
    // Загружаем плейлист в стабильном порядке добавления.
    QStringList result;
    if (!ensureOpened()) {
        return result;
    }

    QSqlQuery query(database());
    if (!query.exec("SELECT file_path FROM playlist ORDER BY id ASC")) {
        qWarning() << "PlaylistRepository: ошибка чтения плейлиста:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        result << query.value(0).toString();
    }
    return result;
}

bool PlaylistRepository::addTrack(const QString &filePath)
{
    // Дубликаты игнорируем на уровне SQL (INSERT OR IGNORE).
    if (!ensureOpened()) {
        return false;
    }

    QSqlQuery query(database());
    query.prepare("INSERT OR IGNORE INTO playlist(file_path) VALUES(:path)");
    query.bindValue(":path", filePath);
    if (!query.exec()) {
        qWarning() << "PlaylistRepository: ошибка добавления трека:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PlaylistRepository::removeTrack(const QString &filePath)
{
    // Удаляем трек по полному пути/URL.
    if (!ensureOpened()) {
        return false;
    }

    QSqlQuery query(database());
    query.prepare("DELETE FROM playlist WHERE file_path = :path");
    query.bindValue(":path", filePath);
    if (!query.exec()) {
        qWarning() << "PlaylistRepository: ошибка удаления трека:" << query.lastError().text();
        return false;
    }
    return true;
}

QString PlaylistRepository::connectionName() const
{
    // Фиксированное имя подключения, чтобы не плодить несколько БД-сессий.
    return "playlist_connection";
}

QSqlDatabase PlaylistRepository::database() const
{
    // Достаем уже зарегистрированное именованное подключение.
    return QSqlDatabase::database(connectionName());
}

bool PlaylistRepository::ensureOpened() const
{
    // Проверяем валидность подключения и при необходимости открываем его.
    QSqlDatabase db = database();
    if (!db.isValid()) {
        qWarning() << "PlaylistRepository: невалидное подключение к БД";
        return false;
    }
    if (db.isOpen()) {
        return true;
    }
    if (!db.open()) {
        qWarning() << "PlaylistRepository: ошибка открытия БД:" << db.lastError().text();
        return false;
    }
    return true;
}
