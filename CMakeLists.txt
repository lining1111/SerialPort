cmake_minimum_required(VERSION 3.19)
project(qt_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_PREFIX_PATH "/home/lining/Qt5.14.2/5.14.2/gcc_64")

##激活自动生成moc文件(通过qmake编译)选项
set(CMAKE_AUTOMOC ON)
##激活自动处理rcc文件(即资源文件，通过qmake编译)选项
set(CMAKE_AUTORCC ON)
##激活自动处理ui文件(通过qmake编译)选项
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)

include_directories(${PROJECT_SOURCE_DIR})

set(SRC_FILES res.qrc
        mainwindow.cpp
        serialport.cpp
        thread.cpp
        pn532.cpp)


add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})

add_subdirectory(test)