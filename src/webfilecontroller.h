/**
 * @file webfilecontroller.h
 * @author Наркевич Алексей
 * @version 1.0
 * @brief WASM-контроллер выбора локального аудиофайла через браузер.
 */

#ifndef WEBFILECONTROLLER_H
#define WEBFILECONTROLLER_H

#include <QObject>

/**
 * @class WebFileController
 * @brief Предоставляет QML API для выбора аудиофайла в WebAssembly.
 *
 * На wasm использует QFileDialog::getOpenFileContent, получает байты файла,
 * сохраняет их во внутреннее хранилище приложения и отдает URL для плеера.
 */
class WebFileController : public QObject
{
    Q_OBJECT
public:
    explicit WebFileController(QObject *parent = nullptr);

    /**
     * @brief Открывает браузерный диалог выбора аудиофайла (только wasm).
     */
    Q_INVOKABLE void pickAudioFile();

signals:
    /**
     * @brief Сигнал успешного выбора и подготовки файла к воспроизведению.
     * @param fileUrl URL сохраненного файла.
     * @param title Человекочитаемое имя трека.
     */
    void fileChosen(const QString &fileUrl, const QString &title);

    /**
     * @brief Сигнал ошибки выбора/сохранения файла.
     * @param message Текст ошибки.
     */
    void errorOccurred(const QString &message);
};

#endif // WEBFILECONTROLLER_H
