################################################################################
#
# Rhythm Runner project CPE 476
# last modified: Wed Jan 18 11:55:28 PST 2017
# Braden Beck (bnbeck)
#
################################################################################
cmake_minimum_required(VERSION 2.8)
project(RhythmRunner CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/game_state)
include_directories(${CMAKE_SOURCE_DIR}/src/game_renderer)
include_directories(${CMAKE_SOURCE_DIR}/src/game_updater)
include_directories(${CMAKE_SOURCE_DIR}/src/helpers)
include_directories(${CMAKE_SOURCE_DIR}/src/generator)
file(GLOB_RECURSE GLSL "assets/shaders/*.glsl")
if(NOT CMAKE_BUILD_TYPE MATCHES Release)
   add_definitions(-DDEBUG)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads)

# GLM - header-only library, just add as an include directory
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm")
include_directories(${GLM_INCLUDE_DIR})

# GLFW
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw")
option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
option(GLFW_INSTALL "GLFW_INSTALL" OFF)
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
else()
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
endif()
include_directories(${GLFW_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# GLEW
set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-cmake")
add_subdirectory(${GLEW_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} libglew_static)
include_directories("${GLEW_DIR}/include")

#SFML
set(SFML-DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/sfml-audio")
add_subdirectory(${SFML-DIR})
if(WIN32)
  SET(RELEASE_DLLS
    lib/glut/bin/glut32.dll
    lib/glew/bin/glew32.dll
    lib/sfml-audio/extlibs/bin/x86/libsndfile-1.dll
    lib/sfml-audio/extlibs/bin/x86/openal32.dll)
endif()
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-audio)

# Aqila
set(AQUILA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/aquila")
option(Aquila_BUILD_EXAMPLES "Aquila_BUILD_EXAMPLES" OFF)
option(Aquila_BUILD_TESTS "Aquila_BUILD_TESTS" OFF)
add_subdirectory(${AQUILA_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} Aquila)
include_directories(${AQUILA_DIR}) # TODO(jarhar): this is very hacky
# On Windows the library location must be explicitly specified
#if(WIN32)
#   if(AQUILA_ROOT)
#      set(AQUILA_INCLUDE_DIR "${AQUILA_ROOT}\\include")
#      set(AQUILA_LIB_DIR "${AQUILA_ROOT}\\lib")
#      message("AQUILA_ROOT: ${AQUILA_ROOT}")
#      message("AQUILA_INCLUDE_DIR: ${AQUILA_INCLUDE_DIR}")
#      message("AQUILA_LIB_DIR: ${AQUILA_LIB_DIR}")
#      include_directories(${AQUILA_INCLUDE_DIR})
#      link_directories(${AQUILA_LIB_DIR})
#   else()
#      message(FATAL_ERROR "Define AQUILA_ROOT when calling cmake.")
#   endif()
#endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif()

# OS specific options and libraries
if(WIN32)
  # c++0x is enabled by default.
  # -Wall produces way too many warnings.
  # -pedantic is not supported.
  # don't know what to do about all the libs I have :(
else()
  # Enable all pedantic warnings.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")
  if(APPLE)
    #set(ENV{CPLUS_INCLUDE_PATH} "${HOME}/.local/include")
    #message($ENV{CPLUS_INCLUDE_PATH})
    # Add required frameworks for GLFW.
    target_link_libraries(${CMAKE_PROJECT_NAME} "-L/usr/local/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lsfml-audio")
  else()
    # Linux
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
    find_package(Sndfile)
    target_link_libraries(${CMAKE_PROJECT_NAME} "GL")
  endif()
endif()
