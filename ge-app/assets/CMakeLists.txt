find_package(Python3 REQUIRED)

function(raw_audio TARGET_NAME RAW_FILE)
    add_custom_command(
        OUTPUT
            ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.c
            ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.h
        COMMAND
            ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.py
            ${RAW_FILE} ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.c
            ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.h ${TARGET_NAME}
        DEPENDS ${RAW_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.py
    )
endfunction()

raw_audio(bgm_ambient ${CMAKE_CURRENT_SOURCE_DIR}/ambient-bgm.raw)
raw_audio(bgm_menu ${CMAKE_CURRENT_SOURCE_DIR}/menu-bgm.raw)

function(texture TARGET_NAME IMAGE_FILE)
    add_custom_command(
        OUTPUT
            ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.c
            ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}.h
        COMMAND
            ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bin2c_texture.py
            ${IMAGE_FILE} ${CMAKE_BINARY_DIR}/generated/${TARGET_NAME}
            ${TARGET_NAME}
        DEPENDS ${IMAGE_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/bin2c_texture.py
    )
endfunction()

texture(default-boat ${CMAKE_CURRENT_SOURCE_DIR}/default-boat.png)
texture(sun ${CMAKE_CURRENT_SOURCE_DIR}/sun.png)
texture(compass-base ${CMAKE_CURRENT_SOURCE_DIR}/compass-base.png)
texture(compass-needle ${CMAKE_CURRENT_SOURCE_DIR}/compass-needle.png)
texture(crate ${CMAKE_CURRENT_SOURCE_DIR}/crate.png)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/generated/font_proggy.c
        ${CMAKE_BINARY_DIR}/generated/font_proggy.h
    COMMAND
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/ttf_to_bitmap_font.py
        --ttf ${CMAKE_CURRENT_SOURCE_DIR}/ProggyClean.ttf --symbol font_proggy
        --out ${CMAKE_BINARY_DIR}/generated/font_proggy
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/ProggyClean.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/ttf_to_bitmap_font.py
        ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.py
)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/generated/bg_clouds.c
        ${CMAKE_BINARY_DIR}/generated/bg_clouds.h
    COMMAND
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_clouds.py
        ${CMAKE_CURRENT_SOURCE_DIR}/clouds.png
        ${CMAKE_BINARY_DIR}/generated/bg_clouds
    DEPENDS
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/gen_clouds.py
        ${CMAKE_CURRENT_SOURCE_DIR}/clouds.png
)

add_library(
    ge-assets
    STATIC
    ${CMAKE_BINARY_DIR}/generated/crate.c
    ${CMAKE_BINARY_DIR}/generated/compass-base.c
    ${CMAKE_BINARY_DIR}/generated/compass-needle.c
    ${CMAKE_BINARY_DIR}/generated/bgm_menu.c
    ${CMAKE_BINARY_DIR}/generated/bg_clouds.c
    ${CMAKE_BINARY_DIR}/generated/sun.c
    ${CMAKE_BINARY_DIR}/generated/bgm_ambient.c
    ${CMAKE_BINARY_DIR}/generated/font_proggy.c
    ${CMAKE_BINARY_DIR}/generated/default-boat.c
)
target_include_directories(ge-assets PUBLIC ${CMAKE_BINARY_DIR}/generated)
