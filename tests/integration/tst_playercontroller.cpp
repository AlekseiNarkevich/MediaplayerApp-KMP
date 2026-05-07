/**
 * @file tst_playercontroller.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Интеграционные тесты взаимодействия контроллера и БД.
 */

#include <QtTest>

#include "../../src/playercontroller.h"

class PlayerControllerIntegrationTests : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief Проверяет добавление в плейлист и синхронизацию свойства.
     */
    void testPlaylistAdd();

    /**
     * @brief Проверяет удаление из плейлиста.
     */
    void testPlaylistRemove();

    /**
     * @brief Проверяет переключение текущего трека.
     */
    void testCurrentTrackChange();

    /**
     * @brief Проверяет, что дубликат трека не добавляется в плейлист.
     */
    void testDuplicateTrackIgnored();

    /**
     * @brief Проверяет корректную обработку отрицательной перемотки.
     */
    void testNegativeSeekIgnored();
};

void PlayerControllerIntegrationTests::testPlaylistAdd()
{
    // Проверяем, что контроллер добавляет трек в in-memory плейлист.
    PlayerController controller;
    controller.addToPlaylist("C:/music/integration_a.mp3");
    QVERIFY(controller.playlist().contains("C:/music/integration_a.mp3"));
}

void PlayerControllerIntegrationTests::testPlaylistRemove()
{
    // Проверяем удаление трека и синхронизацию состояния плейлиста.
    PlayerController controller;
    controller.addToPlaylist("C:/music/integration_b.mp3");
    controller.removeFromPlaylist("C:/music/integration_b.mp3");
    QVERIFY(!controller.playlist().contains("C:/music/integration_b.mp3"));
}

void PlayerControllerIntegrationTests::testCurrentTrackChange()
{
    // Проверяем, что playTrack обновляет currentTrack.
    PlayerController controller;
    controller.playTrack("C:/music/integration_c.mp3");
    QCOMPARE(controller.currentTrack(), QString("C:/music/integration_c.mp3"));
}

void PlayerControllerIntegrationTests::testDuplicateTrackIgnored()
{
    // Ожидаем, что повторное добавление того же пути игнорируется.
    PlayerController controller;
    controller.addToPlaylist("C:/music/integration_d.mp3");
    controller.addToPlaylist("C:/music/integration_d.mp3");
    QCOMPARE(controller.playlist().count("C:/music/integration_d.mp3"), 1);
}

void PlayerControllerIntegrationTests::testNegativeSeekIgnored()
{
    // Отрицательная перемотка не должна приводить к некорректной позиции.
    PlayerController controller;
    controller.seek(-1500);
    QVERIFY(controller.position() >= 0);
}

QTEST_MAIN(PlayerControllerIntegrationTests)
#include "tst_playercontroller.moc"
