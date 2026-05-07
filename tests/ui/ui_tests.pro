######################################################################
# @file ui_tests.pro
# @author Наркевич Алексей
# @version 1.0
# @brief qmake-проект UI-тестов (Qt Quick Test).
######################################################################

QT += qml quick testlib qmltest
CONFIG += testcase c++17 console

TEMPLATE = app
TARGET = ui_tests

SOURCES += quicktest_main.cpp

RESOURCES += ui_tests.qrc
