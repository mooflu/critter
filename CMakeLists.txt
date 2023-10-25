cmake_minimum_required(VERSION 3.11)

project(critter C CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/mooflu.common/utilscmake")

if (NOT VERSION)
    find_package (Git)
    if (GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --always --tags "--abbrev=0"
            OUTPUT_VARIABLE VERSION)
    endif (GIT_FOUND)
endif()

if (NOT VERSION)
    message( FATAL_ERROR "VERSION not specified or no git tag found" )
endif()

string(STRIP ${VERSION} VERSION)
message("VERSION: ${VERSION}")

add_definitions(-DPACKAGE="Critter" -DVERSION="${VERSION}" -DSTATIC_LIB)
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(EMSCRIPTEN)
    string(REGEX REPLACE "\n *" " " EMSCRIPTEN_FLAGS "
        -fno-rtti
        -Oz
        -s USE_SDL=2
        -s USE_SDL_IMAGE=2
        -s SDL2_IMAGE_FORMATS='[\"png\"]'
        -s USE_SDL_MIXER=2
        -s USE_ZLIB=1
        -s USE_LIBPNG=1
        -s USE_ICU=1
    ")
#-s USE_OGG=1
#-s USE_VORBIS=1
#-O2
#-fno-exceptions
#-s FULL_ES3=1
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS} -s USE_WEBGL2=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${PROJECT_SOURCE_DIR}/critter-template.html")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${PROJECT_SOURCE_DIR}/3rdparty/glm")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_DEBUG_POSTFIX "_d")

include(${PROJECT_SOURCE_DIR}/mooflu.common/utilscmake/PlatformSetup.cmake)

add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/utils ${CMAKE_BINARY_DIR}/utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/utilsrandom ${CMAKE_BINARY_DIR}/utilsrandom)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/utilssdl ${CMAKE_BINARY_DIR}/utilssdl)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/utilsgl ${CMAKE_BINARY_DIR}/utilsgl)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/utilsfs ${CMAKE_BINARY_DIR}/utilsfs)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/tinyxml ${CMAKE_BINARY_DIR}/tinyxml)
add_subdirectory(${PROJECT_SOURCE_DIR}/mooflu.common/miniyaml ${CMAKE_BINARY_DIR}/miniyaml)
add_subdirectory(${PROJECT_SOURCE_DIR}/game)

set_target_properties(critter PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VS_DEBUGGER_COMMAND "${CMAKE_BINARY_DIR}/game/Debug/critter.exe"
    VS_DEBUGGER_COMMAND_ARGUMENTS "-fullscreen 0 -audio 0"
)

if (APPLE)
    set_target_properties(critter PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Critter"
        MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2004-2023 Frank Becker"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.mooflu.Critter"
        MACOSX_BUNDLE_ICON_FILE "critter"
    )
endif()
