/**
 * @file quicktest_main.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Точка входа UI-тестов Qt Quick Test.
 */

#include <QtGui/QGuiApplication>
#include <QtQuickTest/quicktest.h>

/**
 * @brief Явно запускает Qt Quick Test и указывает источник тестов в ресурсах qrc.
 * @param argc Количество аргументов командной строки.
 * @param argv Аргументы командной строки.
 * @return Код завершения тестового процесса.
 */
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    return quick_test_main(argc, argv, "MediaPlayerUiTests", ":/");
}
