cmake_minimum_required(VERSION 3.0)

set(EXENAME demo)

#external library includes
include_directories("${PROJECT_SOURCE_DIR}/libraries")
message(STATUS "External librabry dir: " ${PROJECT_SOURCE_DIR}/libraries)
   
#external librabry link dir
link_directories(${PROJECT_SOURCE_DIR}/libraries)
     
#mainline include dirs
#REQUIRES (at compile time): 
#  Proland core library version 4.0.
#    Proland terrain plugin version 4.0.
#      Proland ocean plugin version 4.0.
#        Proland forest plugin version 4.0.
#          Proland atmo plugin version 4.0.
#            Proland edit plugin version 4.0.
#              Proland river plugin version 4.0.
#                Proland graph plugin version 4.0.
include_directories(${PROLAND_CORE_SOURCES} ${PROLAND_TERRAIN_SOURCES} ${PROLAND_GRAPH_SOURCES} ${PROLAND_EDIT_SOURCES} ${PROLAND_ATMO_SOURCES} ${PROLAND_OCEAN_SOURCES} ${PROLAND_RIVER_SOURCES} ${PROLAND_FOREST_SOURCES} )
# Sources
file(GLOB SOURCE_FILES *.cpp)

FIND_PACKAGE(glfw3 REQUIRED)

if(CMAKE_COMPILER_ID STREQUAL "MSVC")
    add_definitions("-DORK_API=__declspec(dllimport)" "-DTIXML_USE_STL" "-DPROLAND_API=__declspec(dllimport)" "-DUSE_SHARED_PTR")
else()
    add_definitions("-DTIXML_USE_STL" "-DUSE_SHARED_PTR" "-fPIC")
    add_compile_definitions("ORK_API=")
    add_compile_definitions("PROLAND_API=")
endif()

set(EXAMPLE_EXE_PATH "/../demo")
set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}${EXAMPLE_EXE_PATH}")
message(STATUS "Setting example output dir: " ${EXECUTABLE_OUTPUT_PATH})


add_executable(${EXENAME} ${SOURCE_FILES})
IF(WIN32)
    target_link_libraries(${EXENAME} proland-core proland-terrain proland-graph proland-edit proland-atmo proland-ocean proland-river proland-forest ork pthreadVC2 OpenGL32 GLU32 glew32 freeglut glfw3 tiff AntTweakBar64 stb_image tinyxml)
ELSE(WIN32)
    target_link_libraries(${EXENAME}  -Wl,--whole-archive proland-core proland-terrain proland-graph proland-edit proland-atmo proland-ocean proland-river proland-forest ork -Wl,--no-whole-archive pthread GL GLU GLEW glut glfw rt dl Xrandr Xinerama Xxf86vm Xext Xcursor Xrender Xfixes tiff AntTweakBar stb_image tinyxml X11)
ENDIF(WIN32)



