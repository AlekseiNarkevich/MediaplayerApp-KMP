######################################################################
# @file unit_tests.pro
# @author Наркевич Алексей
# @version 1.0
# @brief qmake-проект модульных тестов.
######################################################################

QT += testlib sql multimedia
CONFIG += testcase c++17 console

TEMPLATE = app
TARGET = unit_tests

SOURCES += \
    tst_repository.cpp \
    ../../src/playlistrepository.cpp

HEADERS += \
    ../../src/playlistrepository.h
