# ============================================================
# 装甲核心：休闲坦克大战 — CMake 构建文件
# 兼容 Qt 安装器 (C:/Qt/6.x.x/mingw_64) 和 vcpkg 包管理器
# ============================================================
cmake_minimum_required(VERSION 3.16)
project(TankGameMerged VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# UTF-8 编码支持 (MinGW 中文注释/字符串)
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

# 防止 windows.h 的 max/min 宏污染 std::max/std::min
add_compile_definitions(NOMINMAX)

# ---- Qt6 依赖 ----
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Sql)

# ---- 收集源文件 ----
set(CORE_SOURCES
    src/core/game_engine.cpp
    src/core/tank.cpp
    src/core/player_tank.cpp
    src/core/enemy_tank.cpp
    src/core/bullet.cpp
    src/core/base.cpp
    src/core/wall.cpp
    src/core/powerup.cpp
    src/core/skill.cpp
    src/core/map.cpp
)

set(AI_SOURCES
    src/ai/ai_controller.cpp
    src/ai/decision_tree.cpp
    src/ai/patrol_ai.cpp
    src/ai/aggressive_ai.cpp
)

set(UI_SOURCES
    src/ui/game_view.cpp
    src/ui/game_scene.cpp
    src/ui/tank_item.cpp
    src/ui/hud.cpp
)

set(AUDIO_SOURCES
    src/audio/sound_manager.cpp
)

set(DATA_SOURCES
    src/data/config_manager.cpp
    src/data/level_loader.cpp
    src/data/highscore_manager.cpp
)

set(UTILS_SOURCES
    src/utils/resource_manager.cpp
    src/utils/factory.cpp
)

set(APP_SOURCES
    src/app/game_application.cpp
    src/app/main_menu.cpp
    src/app/settings_dialog.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${APP_SOURCES}
    ${CORE_SOURCES}
    ${AI_SOURCES}
    ${UI_SOURCES}
    ${AUDIO_SOURCES}
    ${DATA_SOURCES}
    ${UTILS_SOURCES}
)

# ---- 可执行文件 ----
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Sql
)

# ---- Include 目录 ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ai
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/src/data
    ${CMAKE_SOURCE_DIR}/src/utils
)

# ---- 平台设置 ----
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# ---- 复制 data/ 到构建目录 ----
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

# ---- 单元测试 (Google Test, 可选) ----
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    add_subdirectory(tests)
endif()

# ---- 安装规则 ----
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/TankGameMerged/data)
