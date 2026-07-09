cmake_minimum_required(VERSION 3.22)
project(TankBattle VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt MOC auto-processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Source files (flat layout, all in root)
set(SOURCES
    main.cpp
    mainwindow.cpp
    gamescene.cpp
    startscene.cpp
    tank.cpp
    playertank.cpp
    enemytank.cpp
    bullet.cpp
    wall.cpp
    base.cpp
    gamemanager.cpp
)

set(HEADERS
    constants.h
    mapdata.h
    mainwindow.h
    gamescene.h
    startscene.h
    tank.h
    playertank.h
    enemytank.h
    bullet.h
    wall.h
    base.h
    gamemanager.h
)

add_executable(TankBattle ${SOURCES} ${HEADERS})

target_link_libraries(TankBattle PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Ensure headers in the source dir are found by MOC and compiler
target_include_directories(TankBattle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Note: vcpkg auto-deploys Qt DLLs into the build directory during linking.
# For the Qt platform plugin (qwindows.dll) and other plugins, build.bat
# copies them from vcpkg_installed/ into the build directory before launch.
