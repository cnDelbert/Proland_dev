set(LIBNAME "proland-terrain")

# Sources
message(STATUS "****    Proland terrain    ****")
message(STATUS "Project Source Dir: " ${PROJECT_SOURCE_DIR})
set(PROLAND_TERRAIN_SOURCES "${PROJECT_SOURCE_DIR}/terrain/sources" CACHE INTERNAL "")
include_directories("${PROLAND_TERRAIN_SOURCES}")
include_directories(${PROLAND_CORE_SOURCES})
message(STATUS "include_directories: ${PROLAND_TERRAIN_SOURCES}")
message(STATUS "include_directories: ${PROLAND_CORE_SOURCES}")



file(GLOB SOURCE_FILES *.cpp */*.cpp preprocess/terrain/*.cpp)

# Libraries
set(LIBS "")
if(UNIX)
	set(LIBS ${LIBS} rt)
  else(UNIX)
  set(LIBS ${LIBS} proland-core ork tiff)
endif(UNIX)

# Static or shared?
set(LIBTYPE STATIC)
if(BUILD_SHARED)
	set(LIBTYPE SHARED)
endif(BUILD_SHARED)

# Create a static library
add_library(${LIBNAME} ${LIBTYPE} ${SOURCE_FILES})
target_link_libraries(${LIBNAME} ${LIBS})

# Adds SO Version and subversion. To be added to ensure ABI/API compatibility.
#SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES SOVERSION ${ORK_VERSION_MAJOR} VERSION ${ORK_VERSION})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message(STATUS "Output path for " ${LIBNAME} " is " ${LIBRARY_OUTPUT_PATH})

link_directories(${PROJECT_SOURCE_DIR}/lib)

# Install headers
FOREACH(subdir dem ortho preprocess/terrain)
  FILE(GLOB include-files ${subdir}/*.h)
  INSTALL(FILES ${include-files} DESTINATION include/proland/${subdir})
ENDFOREACH(subdir)

# Install library
INSTALL(TARGETS ${LIBNAME} LIBRARY DESTINATION ${LIB_INSTALL_DIR} ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

#SET(ORK_CFLAGS "-DORK_API= -DTIXML_USE_STL")
#SET(PROLAND_CFLAGS "-DPROLAND_API=" "-DUSE_SHARED_PTR")
if(CMAKE_COMPILER_ID STREQUAL "MSVC")
    add_definitions("-DORK_API=__declspec(dllimport)" "-DTIXML_USE_STL" "-DPROLAND_API=__declspec(dllimport)" "-DUSE_SHARED_PTR")
else()
    add_definitions("-DTIXML_USE_STL" "-DUSE_SHARED_PTR" "-fPIC")
    add_compile_definitions("ORK_API=")
    add_compile_definitions("PROLAND_API=")
endif()

CONFIGURE_FILE(proland-terrain.pc.in ${PROJECT_BINARY_DIR}/proland-terrain.pc @ONLY)
INSTALL(FILES ${PROJECT_BINARY_DIR}/proland-terrain.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
