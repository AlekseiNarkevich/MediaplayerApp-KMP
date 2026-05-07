######################################################################
# @file integration_tests.pro
# @author Наркевич Алексей
# @version 1.0
# @brief qmake-проект интеграционных тестов.
######################################################################

QT += testlib sql multimedia
CONFIG += testcase c++17 console

TEMPLATE = app
TARGET = integration_tests

SOURCES += \
    tst_playercontroller.cpp \
    ../../src/playercontroller.cpp \
    ../../src/playlistrepository.cpp

HEADERS += \
    ../../src/playercontroller.h \
    ../../src/playlistrepository.h
