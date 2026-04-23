QT += core testlib

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TARGET = core_tests
TEMPLATE = app

INCLUDEPATH += \
    ../../src \
    ../../src/core

SOURCES += \
    tst_core.cpp \
    ../../src/core/ringbuffer.cpp \
    ../../src/core/samplepack.cpp

HEADERS += \
    ../../src/core/ringbuffer.h \
    ../../src/core/samplepack.h
