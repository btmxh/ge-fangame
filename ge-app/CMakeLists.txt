add_subdirectory(assets)

add_executable(ge-app)

target_sources(
    ge-app
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    # add more .cpp here
)

target_link_libraries(ge-app PRIVATE ge-hal ge-assets)

set_target_properties(
    ge-app
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
