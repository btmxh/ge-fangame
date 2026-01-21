find_package(Python3 REQUIRED)

function(bin2c_generic BIN2C SYMBOL_NAME RESOURCE_FILE)
    cmake_parse_arguments(
        BIN2C # prefix
        "" # no boolean options
        "OUTPUT_BASE" # single-value keywords
        "ARGS" # multi-value keywords
        ${ARGN}
    )

    # ---- OUTPUT_BASE default ----
    if(BIN2C_OUTPUT_BASE)
        set(OUTPUT_BASE "${BIN2C_OUTPUT_BASE}")
    else()
        get_filename_component(_dir "${RESOURCE_FILE}" DIRECTORY)
        get_filename_component(_name "${RESOURCE_FILE}" NAME_WE)
        set(OUTPUT_BASE "${_dir}/${_name}")
    endif()

    set(ADDITIONAL_ARGS ${BIN2C_ARGS})

    # ---- Paths ----
    set(SCRIPT_FILE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/${BIN2C})

    set(HEADER_FILE ${CMAKE_BINARY_DIR}/generated/assets/${OUTPUT_BASE}.h)

    set(SOURCE_FILE ${CMAKE_BINARY_DIR}/generated/assets/${OUTPUT_BASE}.c)

    get_filename_component(
        RESOURCE_FILE_ABSOLUTE
        "${RESOURCE_FILE}"
        ABSOLUTE
        BASE_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}"
    )

    message(STATUS "${BIN2C}: ${RESOURCE_FILE} → ${SOURCE_FILE}")

    add_custom_command(
        OUTPUT ${HEADER_FILE} ${SOURCE_FILE}
        COMMAND
            ${Python3_EXECUTABLE} ${SCRIPT_FILE} ${RESOURCE_FILE_ABSOLUTE}
            ${SOURCE_FILE} ${HEADER_FILE} ${SYMBOL_NAME} ${ADDITIONAL_ARGS}
        DEPENDS
            ${RESOURCE_FILE}
            ${SCRIPT_FILE}
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/bin2c.py
        VERBATIM
    )
endfunction()

function(raw_audio SYMBOL_NAME WAV_FILE)
    bin2c_generic(bin2c_audio.py ${SYMBOL_NAME} ${WAV_FILE})
endfunction()

function(raw_image SYMBOL_NAME IMAGE_FILE)
    bin2c_generic(bin2c_image.py ${SYMBOL_NAME} ${IMAGE_FILE} ARGS rgb565)
endfunction()

function(raw_image_alpha SYMBOL_NAME IMAGE_FILE)
    bin2c_generic(bin2c_image.py ${SYMBOL_NAME} ${IMAGE_FILE} ARGS argb1555)
endfunction()

function(bitmap_font SYMBOL_NAME FONT_FILE FONT_SIZE)
    get_filename_component(_dir ${FONT_FILE} DIRECTORY)
    get_filename_component(_name ${FONT_FILE} NAME_WE)
    bin2c_generic(bin2c_bitmap_font.py ${SYMBOL_NAME} ${FONT_FILE} OUTPUT_BASE "${_dir}/${_name}_${FONT_SIZE}px" ARGS ${FONT_SIZE})
endfunction()

raw_audio(bgm_ambient out/sounds/ambient-bgm.wav)
raw_audio(bgm_menu out/sounds/menu-bgm.wav)

raw_image_alpha(default_boat out/textures/default-boat.png)
raw_image_alpha(sun out/textures/sun.png)
raw_image_alpha(compass_base out/textures/compass-base.png)
raw_image_alpha(compass_needle out/textures/compass-needle.png)
raw_image_alpha(crate out/textures/crate.png)

bitmap_font(font_pixeloid_9px src/fonts/Pixeloid/TTF/PixeloidSans.ttf 9)
bitmap_font(font_pixeloid_9px_bold src/fonts/Pixeloid/TTF/PixeloidSans-Bold.ttf 9)
bitmap_font(font_pixeloid_18px src/fonts/Pixeloid/TTF/PixeloidSans.ttf 18)
bin2c_generic(bin2c_clouds.py bg_clouds out/textures/clouds.png)

add_library(
    ge-assets
    STATIC
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/crate.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/compass-base.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/compass-needle.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/clouds.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/sun.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/textures/default-boat.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/sounds/menu-bgm.c
    ${CMAKE_BINARY_DIR}/generated/assets/out/sounds/ambient-bgm.c
    ${CMAKE_BINARY_DIR}/generated/assets/src/fonts/Pixeloid/TTF/PixeloidSans_9px.c
    ${CMAKE_BINARY_DIR}/generated/assets/src/fonts/Pixeloid/TTF/PixeloidSans-Bold_9px.c
    ${CMAKE_BINARY_DIR}/generated/assets/src/fonts/Pixeloid/TTF/PixeloidSans_18px.c
)
target_include_directories(ge-assets PUBLIC ${CMAKE_BINARY_DIR}/generated)
