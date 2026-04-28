QT += core testlib

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TARGET = core_tests
TEMPLATE = app

INCLUDEPATH += \
    ../../src \
    ../../src/core \
    ../../src/protocol \
    ../../src/storage

SOURCES += \
    tst_core.cpp \
    ../../src/core/ringbuffer.cpp \
    ../../src/core/samplepack.cpp \
    ../../src/protocol/protocolframe.cpp \
    ../../src/protocol/protocolparser.cpp \
    ../../src/storage/csvrecorder.cpp

HEADERS += \
    ../../src/core/ringbuffer.h \
    ../../src/core/samplepack.h \
    ../../src/protocol/protocolframe.h \
    ../../src/protocol/protocolparser.h \
    ../../src/storage/csvrecorder.h
