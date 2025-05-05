cmake_minimum_required(VERSION 3.15...3.30)

project(SoftBodyDemo
    VERSION 1.0
    DESCRIPTION "Demo for soft body simulation"
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(CGAL REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(softbody ${SOURCES})

set_target_properties(softbody PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF)

target_link_libraries(softbody sfml-graphics sfml-window sfml-system ${CURL_LIBRARIES})
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(softbody CGAL::CGAL ${OpenCV_LIBS})

set(FONT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fonts")

file(GLOB FONT_FILES "${FONT_DIR}/*")
foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_FILENAME ${FONT_FILE} NAME)
    add_custom_command(TARGET softbody POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${FONT_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${FONT_FILENAME}"
    )
endforeach()

target_compile_definitions(softbody PRIVATE FONT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
