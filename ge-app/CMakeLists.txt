add_subdirectory(assets)

add_executable(ge-app)

file(GLOB_RECURSE GE_APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE GE_APP_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

target_sources(ge-app PRIVATE ${GE_APP_SOURCES} ${GE_APP_HEADERS})

target_include_directories(ge-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ge-app PRIVATE ge-hal ge-assets)
ge_hal_add_link_sources(ge-app)
