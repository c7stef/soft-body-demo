cmake_minimum_required(VERSION 3.15...3.30)

project(GreedyColoring
    VERSION 1.0
    DESCRIPTION "Greedy coloring visualizer"
    LANGUAGES CXX)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(CURL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(graphdamp ${SOURCES})

set_target_properties(graphdamp PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF)

target_link_libraries(graphdamp sfml-graphics sfml-window sfml-system ${CURL_LIBRARIES})

set(FONT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")

file(GLOB FONT_FILES "${FONT_DIR}/*")
foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_FILENAME ${FONT_FILE} NAME)
    add_custom_command(TARGET graphdamp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${FONT_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${FONT_FILENAME}"
    )
endforeach()

target_compile_definitions(graphdamp PRIVATE FONT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
