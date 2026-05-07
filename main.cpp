/**
 * @file main.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Точка входа кроссплатформенного приложения "Медиапроигрыватель".
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QLocale>

#include "src/languagecontroller.h"
#include "src/medialibrarymodel.h"
#include "src/playercontroller.h"
#include "src/webfilecontroller.h"

/**
 * @brief Точка входа в приложение.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код завершения приложения.
 */
int main(int argc, char *argv[])
{
    // Инициализация графического приложения Qt.
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("MediaPlayer");
    QGuiApplication::setOrganizationName("Narkevich");

    // Регистрируем C++ типы, чтобы их можно было создавать в QML.
    qmlRegisterType<MediaLibraryModel>("MediaPlayer", 1, 0, "MediaLibraryModel");
    qmlRegisterType<PlayerController>("MediaPlayer", 1, 0, "PlayerController");

    QQmlApplicationEngine engine;
    LanguageController languageController(&app, &engine);
    WebFileController webFileController;

    // Определяем стартовый язык по языку системы с fallback на русский.
    const QString localeCode = QLocale::system().name().left(2).toLower();
    if (localeCode == "en" || localeCode == "be") {
        languageController.setLanguage(localeCode);
    } else {
        languageController.setLanguage("ru");
    }

    // Делаем контроллер языка доступным из QML-контекста.
    engine.rootContext()->setContextProperty("languageController", &languageController);
    // Контроллер выбора файла для WebAssembly-сценария.
    engine.rootContext()->setContextProperty("webFileController", &webFileController);
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

    // В случае ошибки загрузки корневого QML-объекта завершаем приложение с кодом -1.
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            qCritical() << "main: ошибка создания корневого QML-объекта";
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
