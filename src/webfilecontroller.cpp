/**
 * @file webfilecontroller.cpp
 * @author Наркевич Алексей
 * @version 1.0
 * @brief Реализация wasm-контроллера выбора файла.
 */

#include "webfilecontroller.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUrl>
#include <QUuid>

#ifdef Q_OS_WASM
#include <QFileDialog>
#endif

WebFileController::WebFileController(QObject *parent)
    : QObject(parent)
{
    // Конструктор без дополнительной инициализации.
}

void WebFileController::pickAudioFile()
{
#ifdef Q_OS_WASM
    // В WebAssembly используем специальный API Qt для чтения файла как QByteArray.
    const QString filter =
        tr("Audio files (*.mp3 *.wav *.flac *.ogg *.m4a *.aac *.opus);;All files (*.*)");

    QFileDialog::getOpenFileContent(filter, [this](const QString &fileName, const QByteArray &fileContent) {
        if (fileName.isEmpty() || fileContent.isEmpty()) {
            return;
        }

        const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (baseDir.isEmpty() || !QDir().mkpath(baseDir)) {
            emit errorOccurred(tr("Не удалось создать каталог для временного файла."));
            return;
        }

        const QString extension = QFileInfo(fileName).suffix();
        const QString uniqueName = QUuid::createUuid().toString(QUuid::WithoutBraces)
                                   + (extension.isEmpty() ? QString() : "." + extension);
        const QString targetPath = QDir(baseDir).absoluteFilePath(uniqueName);

        // Сохраняем данные во внутренний файл, чтобы затем отдать URL в QMediaPlayer.
        QFile outFile(targetPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            emit errorOccurred(tr("Не удалось открыть временный файл для записи."));
            return;
        }
        outFile.write(fileContent);
        outFile.close();

        emit fileChosen(QUrl::fromLocalFile(targetPath).toString(), QFileInfo(fileName).completeBaseName());
    });
#else
    emit errorOccurred(tr("Web file picker доступен только в сборке WebAssembly."));
#endif
}
