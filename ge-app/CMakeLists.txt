add_subdirectory(assets)

add_executable(ge-app)

target_sources(
    ge-app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ge-app/text.cpp
)

target_include_directories(ge-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ge-app PRIVATE ge-hal ge-assets)
ge_hal_add_link_sources(ge-app)
