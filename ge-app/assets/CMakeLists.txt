find_package(Python3 REQUIRED)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/generated/bgm_loop.c
        ${CMAKE_BINARY_DIR}/generated/bgm_loop.h
    COMMAND
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.py
        ${CMAKE_CURRENT_SOURCE_DIR}/loop.bin
        ${CMAKE_BINARY_DIR}/generated/bgm_loop.c
        ${CMAKE_BINARY_DIR}/generated/bgm_loop.h bgm_loop
    DEPENDS loop.bin ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.py
)

add_library(ge-assets STATIC ${CMAKE_BINARY_DIR}/generated/bgm_loop.c)
target_include_directories(ge-assets PUBLIC ${CMAKE_BINARY_DIR}/generated)
