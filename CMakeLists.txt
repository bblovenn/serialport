cmake_minimum_required(VERSION 3.16)

project(serialport VERSION 1.0.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort Test)

set(SERIALPORT_INCLUDE_DIRS
    src
    src/core
    src/protocol
    src/readers
    src/serial
    src/storage
    src/plot
)

set(SERIALPORT_CORE_SOURCES
    src/core/ringbuffer.cpp
    src/core/ringbuffer.h
    src/core/samplepack.cpp
    src/core/samplepack.h
    src/core/stream.cpp
    src/core/stream.h
    src/core/streamchannel.cpp
    src/core/streamchannel.h
    src/protocol/protocolframe.cpp
    src/protocol/protocolframe.h
    src/protocol/protocolparser.cpp
    src/protocol/protocolparser.h
    src/readers/abstractreader.cpp
    src/readers/abstractreader.h
    src/readers/asciireader.cpp
    src/readers/asciireader.h
    src/readers/demoreader.cpp
    src/readers/demoreader.h
    src/serial/serialcontroller.cpp
    src/serial/serialcontroller.h
    src/storage/csvrecorder.cpp
    src/storage/csvrecorder.h
    src/plot/plotwidget.cpp
    src/plot/plotwidget.h
)

add_executable(serialport
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    ${SERIALPORT_CORE_SOURCES}
    resources.qrc
)

target_include_directories(serialport PRIVATE ${SERIALPORT_INCLUDE_DIRS})

target_link_libraries(serialport PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)

if(BUILD_TESTING)
    add_executable(core_tests
        tests/core/tst_core.cpp
        src/core/ringbuffer.cpp
        src/core/ringbuffer.h
        src/core/samplepack.cpp
        src/core/samplepack.h
        src/protocol/protocolframe.cpp
        src/protocol/protocolframe.h
        src/protocol/protocolparser.cpp
        src/protocol/protocolparser.h
        src/storage/csvrecorder.cpp
        src/storage/csvrecorder.h
    )

    target_include_directories(core_tests PRIVATE
        src
        src/core
        src/protocol
        src/storage
    )

    target_link_libraries(core_tests PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Test
    )

    add_test(NAME core_tests COMMAND core_tests)

    add_executable(ui_layout_test
        tests/ui/tst_mainwindowlayout.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        ${SERIALPORT_CORE_SOURCES}
    )

    target_include_directories(ui_layout_test PRIVATE ${SERIALPORT_INCLUDE_DIRS})

    target_link_libraries(ui_layout_test PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::SerialPort
        Qt5::Test
    )

    add_test(NAME ui_layout_test COMMAND ui_layout_test)
    set_tests_properties(ui_layout_test PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QPA_FONTDIR=C:/Windows/Fonts;QT_LOGGING_RULES=*.debug=false"
    )
endif()
