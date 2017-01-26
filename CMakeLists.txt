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
file(GLOB_RECURSE GLSL "assets/shaders/*.glsl")

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
if(CMAKE_BUILD_TYPE MATCHES Release)
   add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
else()
   add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
endif()
include_directories(${GLFW_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# GLEW
#set(GLEW_DIR "$ENV{GLEW_DIR}")
set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glew")
include_directories(${GLEW_DIR}/include)
if(WIN32)
   # With prebuilt binaries
   target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/Release/Win32/glew32s.lib)
else()
   target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/libGLEW.a)
endif()

# Aqila
target_link_libraries(${CMAKE_PROJECT_NAME} Aquila)
# On Windows the library location must be explicitly specified
if(WIN32)
   if(AQUILA_ROOT)
      set(AQUILA_INCLUDE_DIR "${AQUILA_ROOT}\\include")
      set(AQUILA_LIB_DIR "${AQUILA_ROOT}\\lib")
      message("AQUILA_ROOT: ${AQUILA_ROOT}")
      message("AQUILA_INCLUDE_DIR: ${AQUILA_INCLUDE_DIR}")
      message("AQUILA_LIB_DIR: ${AQUILA_LIB_DIR}")
      include_directories(${AQUILA_INCLUDE_DIR})
      link_directories(${AQUILA_LIB_DIR})
   else()
      message(FATAL_ERROR "Define AQUILA_ROOT when calling cmake.")
   endif()
endif()

# There should be a CMakeLists.txt in the specified directory.
set(GLFW_DIR "deps/glfw")
if(NOT GLFW_DIR)
   message(FATAL_ERROR "deps/glfw not found. Did you run git submodule init && git submodule update?")
endif()
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
      target_link_libraries(${CMAKE_PROJECT_NAME} "-L/usr/local/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework SFML -lsfml-audio -lsfml-window -lsfml-graphics -lsfml-network -lsfml-system")
   else()
      # Linux
      target_link_libraries(${CMAKE_PROJECT_NAME} "GL")

      # SFML
      set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
      find_package(SFML 2 REQUIRED system window graphics network audio)
      if(SFML_FOUND)
        include_directories(${SFML_INCLUDE_DIR})
        target_link_libraries(${CMAKE_PROJECT_NAME} ${SFML_LIBRARIES})
      endif()
   endif()
endif()
