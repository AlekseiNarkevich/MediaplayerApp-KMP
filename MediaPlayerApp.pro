######################################################################
# @file MediaPlayerApp.pro
# @author Наркевич Алексей
# @version 1.0
# @brief qmake-проект кроссплатформенного приложения "Медиапроигрыватель".
######################################################################

QT += quick qml multimedia sql testlib
wasm: QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    src/languagecontroller.cpp \
    src/medialibrarymodel.cpp \
    src/webfilecontroller.cpp \
    src/playlistrepository.cpp \
    src/playercontroller.cpp

HEADERS += \
    src/languagecontroller.h \
    src/medialibrarymodel.h \
    src/webfilecontroller.h \
    src/playlistrepository.h \
    src/playercontroller.h

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    i18n/app_ru.ts \
    i18n/app_en.ts \
    i18n/app_be.ts
