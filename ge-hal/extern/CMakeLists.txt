add_library(cmsis INTERFACE)
target_include_directories(
    cmsis
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_5/CMSIS/Core/Include
)

add_library(cmsis_device_f4 INTERFACE)
target_include_directories(
    cmsis_device_f4
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_device_f4/Include
)
