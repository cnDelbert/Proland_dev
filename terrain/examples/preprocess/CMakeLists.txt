cmake_minimum_required(VERSION 3.0)

set(EXENAME preprocess)

#external library includes
include_directories("${PROJECT_SOURCE_DIR}/libraries")
message(STATUS "External librabry dir: " ${PROJECT_SOURCE_DIR}/libraries)

#external librabry link dir
link_directories(${PROJECT_SOURCE_DIR}/libraries)

#mainline include dirs
include_directories(${PROLAND_TERRAIN_SOURCES} ${PROLAND_CORE_SOURCES})

#sources
file(GLOB SOURCE_FILES *.cpp)

if(CMAKE_COMPILER_ID STREQUAL "MSVC")
    add_definitions("-DORK_API=__declspec(dllimport)" "-DTIXML_USE_STL" "-DPROLAND_API=__declspec(dllimport)" "-DUSE_SHARED_PTR")
else()
    add_definitions("-DTIXML_USE_STL" "-DUSE_SHARED_PTR" "-fPIC")
    add_compile_definitions("ORK_API=")
    add_compile_definitions("PROLAND_API=")
endif()

set(EXAMPLE_EXE_PATH "/examples/terrain/preprocess")
set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}${EXAMPLE_EXE_PATH}")
message(STATUS "Setting example output dir: " ${EXECUTABLE_OUTPUT_PATH})


add_executable(${EXENAME} ${SOURCE_FILES})
IF(WIN32)
    target_link_libraries(${EXENAME} proland-core proland-terrain ork pthreadVC2 OpenGL32 GLU32 glew32 freeglut glfw3 tiff AntTweakBar64 stb_image tinyxml)
ELSE(WIN32)
    target_link_libraries(${EXENAME} -Wl,--whole-archive proland-core proland-terrain ork -Wl,--no-whole-archive pthread GL GLU GLEW glut glfw rt dl Xrandr Xinerama Xxf86vm Xext Xcursor Xrender Xfixes tiff AntTweakBar stb_image tinyxml X11)
ENDIF(WIN32)

# Copy all files in source tree, except this CMakeLists.txt and source files
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${EXECUTABLE_OUTPUT_PATH})
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/CMakeLists.txt)
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/*.h)
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/*.cpp)

