/**
 * @file languagecontroller.h
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Контроллер переключения языка интерфейса в рантайме.
 */

#ifndef LANGUAGECONTROLLER_H
#define LANGUAGECONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QTranslator>

class QGuiApplication;
class QQmlEngine;

/**
 * @class LanguageController
 * @brief Управляет загрузкой переводов и обновлением QML-строк.
 */
class LanguageController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentLanguage READ currentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages CONSTANT)

public:
    /**
     * @brief Создает контроллер переключения языка.
     * @param app Указатель на приложение (для installTranslator/removeTranslator).
     * @param engine QML-движок для retranslate().
     * @param parent Родительский QObject.
     */
    explicit LanguageController(QGuiApplication *app, QQmlEngine *engine, QObject *parent = nullptr);

    /**
     * @brief Возвращает текущий языковой код (например, ru, en, be).
     */
    QString currentLanguage() const;

    /**
     * @brief Возвращает список поддерживаемых языков.
     */
    QStringList availableLanguages() const;

    /**
     * @brief Применяет язык интерфейса.
     * @param languageCode Код языка (ru, en, be).
     */
    Q_INVOKABLE void setLanguage(const QString &languageCode);

signals:
    /**
     * @brief Сигнал изменения текущего языка.
     */
    void currentLanguageChanged();

private:
    QGuiApplication *m_app;
    QQmlEngine *m_engine;
    QTranslator m_translator;
    // Пустое значение важно: первый вызов setLanguage("ru") должен реально загрузить перевод.
    QString m_currentLanguage = "";
};

#endif // LANGUAGECONTROLLER_H
