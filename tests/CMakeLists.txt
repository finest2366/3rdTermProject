cmake_minimum_required(VERSION 3.16)

set(TEST_SOURCES test_tank.cpp)

# 项目源码（测试需要链接，不含 main.cpp）
set(PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/game_engine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/tank.cpp
    ${CMAKE_SOURCE_DIR}/src/core/player_tank.cpp
    ${CMAKE_SOURCE_DIR}/src/core/enemy_tank.cpp
    ${CMAKE_SOURCE_DIR}/src/core/bullet.cpp
    ${CMAKE_SOURCE_DIR}/src/core/base.cpp
    ${CMAKE_SOURCE_DIR}/src/core/wall.cpp
    ${CMAKE_SOURCE_DIR}/src/core/powerup.cpp
    ${CMAKE_SOURCE_DIR}/src/core/skill.cpp
    ${CMAKE_SOURCE_DIR}/src/core/map.cpp
    ${CMAKE_SOURCE_DIR}/src/ai/ai_controller.cpp
    ${CMAKE_SOURCE_DIR}/src/ai/decision_tree.cpp
    ${CMAKE_SOURCE_DIR}/src/ai/patrol_ai.cpp
    ${CMAKE_SOURCE_DIR}/src/ai/aggressive_ai.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/game_view.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/game_scene.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/tank_item.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/hud.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/sound_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/data/config_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/data/level_loader.cpp
    ${CMAKE_SOURCE_DIR}/src/data/highscore_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/resource_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/factory.cpp
    ${CMAKE_SOURCE_DIR}/src/app/game_application.cpp
    ${CMAKE_SOURCE_DIR}/src/app/main_menu.cpp
    ${CMAKE_SOURCE_DIR}/src/app/settings_dialog.cpp
)

add_executable(tank_tests ${TEST_SOURCES} ${PROJECT_SOURCES})

target_link_libraries(tank_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Sql
)

target_include_directories(tank_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ai
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/data
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/src/app
)

include(GoogleTest)
gtest_discover_tests(tank_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:C:/Qt/6.11.1/mingw_64/bin\;C:/Qt/Tools/mingw1310_64/bin"
)
