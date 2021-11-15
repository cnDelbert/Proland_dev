cmake_minimum_required(VERSION 3.0)

set(EXENAME river1)

#external library includes
include_directories("${PROJECT_SOURCE_DIR}/libraries")
message(STATUS "External librabry dir: " ${PROJECT_SOURCE_DIR}/libraries)
   
#external librabry link dir
link_directories(${PROJECT_SOURCE_DIR}/libraries)
     
#mainline include dirs
include_directories(${PROLAND_TERRAIN_SOURCES} ${PROLAND_CORE_SOURCES} ${PROLAND_GRAPH_SOURCES} ${PROLAND_RIVER_SOURCES})

# Sources
file(GLOB SOURCE_FILES *.cpp)

add_definitions("-DORK_API=__declspec(dllimport)" "-DTIXML_USE_STL" "-DPROLAND_API=__declspec(dllimport)" "-DUSE_SHARED_PTR")

set(EXAMPLE_EXE_PATH "/examples/river/river1")
set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}${EXAMPLE_EXE_PATH}")
message(STATUS "Setting example output dir: " ${EXECUTABLE_OUTPUT_PATH})


add_executable(${EXENAME} ${SOURCE_FILES})
IF(WIN32)
    target_link_libraries(${EXENAME} proland-core proland-terrain proland-graph proland-river ork pthreadVC2 OpenGL32 GLU32 glew32 freeglut glfw3 tiff AntTweakBar64 stb_image tinyxml)
ELSE(WIN32)
    target_link_libraries(${EXENAME}  -Wl,--whole-archive proland-core proland-terrain proland-graph proland-river ork -Wl,--no-whole-archive pthread GL GLU GLEW glut glfw3 rt dl Xrandr Xinerama Xxf86vm Xext Xcursor Xrender Xfixes tiff AntTweakBar stb_image tinyxml X11)
ENDIF(WIN32)

# Copy all files in source tree, except this CMakeLists.txt and source files
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${EXECUTABLE_OUTPUT_PATH})
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/CMakeLists.txt)
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/*.h)
add_custom_command(TARGET ${EXENAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${EXECUTABLE_OUTPUT_PATH}/*.cpp)


