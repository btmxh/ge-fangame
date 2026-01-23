add_subdirectory(assets)

add_executable(ge-app)

target_sources(
    ge-app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/font.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/assets/bgm.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/game_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/menu_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/settings_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/credits_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/management_menu_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/status_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/inventory_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/dialog_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/root_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/scenes/root_management_ui_scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/game/inventory.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/game/player_stats.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/game/tutorial_system.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/ui/menu.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/ui/slider.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ge-app/ui/option_selector.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/font.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/game/water.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/assets/bgm.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/scenes/management_menu_scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/scenes/status_scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/scenes/inventory_scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/root_management_ui_scene.cpp
)

target_include_directories(ge-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ge-app PRIVATE ge-hal ge-assets)
ge_hal_add_link_sources(ge-app)
