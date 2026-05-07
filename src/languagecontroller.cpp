/**
 * @file languagecontroller.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Реализация контроллера переключения языка интерфейса.
 */

#include "languagecontroller.h"

#include <QCoreApplication>
#include <QDir>
#include <QGuiApplication>
#include <QDebug>
#include <QQmlEngine>

LanguageController::LanguageController(QGuiApplication *app, QQmlEngine *engine, QObject *parent)
    : QObject(parent), m_app(app), m_engine(engine)
{
    // Инициализация простая: ссылки на app/engine храним для работы с переводами.
}

QString LanguageController::currentLanguage() const
{
    // Возвращаем текущий активный код языка для UI и логики.
    return m_currentLanguage;
}

QStringList LanguageController::availableLanguages() const
{
    // Поддерживаемые коды языков для UI-переключателя.
    return {"ru", "en", "be"};
}

void LanguageController::setLanguage(const QString &languageCode)
{
    const QString normalized = languageCode.trimmed().toLower();
    // Пропускаем пустой код и повторное применение уже активного языка.
    if (normalized.isEmpty() || normalized == m_currentLanguage) {
        return;
    }

    QString fileName = "app_ru.qm";
    if (normalized == "en") {
        fileName = "app_en.qm";
    } else if (normalized == "be") {
        fileName = "app_be.qm";
    }

    // Ищем qm-файлы в наиболее вероятных папках (debug/release/корень проекта).
    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidateFolders = {
        appDir + "/i18n",
        QDir(appDir).absoluteFilePath("../i18n"),
        QDir(appDir).absoluteFilePath("../../i18n"),
        QDir(appDir).absoluteFilePath("../../../i18n")
    };

    QString loadedFromFolder;
    for (const QString &folder : candidateFolders) {
        QTranslator probeTranslator;
        if (probeTranslator.load(fileName, folder)) {
            loadedFromFolder = folder;
            break;
        }
    }

    m_app->removeTranslator(&m_translator);

    bool loaded = false;
    if (!loadedFromFolder.isEmpty()) {
        loaded = m_translator.load(fileName, loadedFromFolder);
    } else {
        // Резервный сценарий: загрузка перевода из ресурсов приложения.
        loaded = m_translator.load(":/i18n/" + fileName);
    }

    if (!loaded) {
        qWarning() << "LanguageController: не удалось загрузить перевод:" << fileName
                   << "папка запуска:" << appDir;
        return;
    }

    m_app->installTranslator(&m_translator);

    // Сохраняем новое состояние и принудительно обновляем qsTr-строки в QML.
    m_currentLanguage = normalized;
    emit currentLanguageChanged();

    if (m_engine) {
        m_engine->retranslate();
    }
}
