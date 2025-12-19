add_library(ge-hal STATIC)

if(GE_HAL_STM32)
    message(FATAL_ERROR "STM32 platform not yet supported in ge-hal")
else()
    target_sources(ge-hal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/app_pc.cpp)
    find_package(SDL3 REQUIRED)
    target_link_libraries(ge-hal PUBLIC SDL3::SDL3)
endif()

target_include_directories(ge-hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
