/**
 * @file tst_repository.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Модульные тесты репозитория плейлиста.
 */

#include <QtTest>

#include "../../src/playlistrepository.h"

class RepositoryUnitTests : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief Проверяет, что БД и таблица создаются корректно.
     */
    void testInitialize();

    /**
     * @brief Проверяет добавление и чтение трека.
     */
    void testAddAndLoadTrack();

    /**
     * @brief Проверяет удаление трека.
     */
    void testRemoveTrack();
};

void RepositoryUnitTests::testInitialize()
{
    // Проверяем, что репозиторий может создать/открыть БД и таблицу.
    PlaylistRepository repository;
    QVERIFY(repository.initialize());
}

void RepositoryUnitTests::testAddAndLoadTrack()
{
    // Добавляем тестовый трек и убеждаемся, что он читается обратно.
    PlaylistRepository repository;
    QVERIFY(repository.initialize());
    QVERIFY(repository.addTrack("C:/music/sample_track.mp3"));

    const QStringList playlist = repository.loadPlaylist();
    QVERIFY(playlist.contains("C:/music/sample_track.mp3"));
}

void RepositoryUnitTests::testRemoveTrack()
{
    // Проверяем полный цикл: добавить -> удалить -> убедиться, что отсутствует.
    PlaylistRepository repository;
    QVERIFY(repository.initialize());
    QVERIFY(repository.addTrack("C:/music/removable_track.mp3"));
    QVERIFY(repository.removeTrack("C:/music/removable_track.mp3"));

    const QStringList playlist = repository.loadPlaylist();
    QVERIFY(!playlist.contains("C:/music/removable_track.mp3"));
}

QTEST_MAIN(RepositoryUnitTests)
#include "tst_repository.moc"
