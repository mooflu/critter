include(${PROJECT_SOURCE_DIR}/mooflu.common/utilscmake/FindExtras.cmake)

# set(CMAKE_FIND_DEBUG_MODE TRUE)

if(NOT EMSCRIPTEN)
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(PhysFS REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif()

set(RESOURCE_DAT ${PROJECT_SOURCE_DIR}/resource.dat)

set(EXTRA_LIBRARIES "")
if(WIN32)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/Critter.rc)
    string(REPLACE "." "," VERSION_COMMA ${VERSION})
    set_source_files_properties(${APP_ICON} PROPERTIES
        COMPILE_DEFINITIONS "
            VER_LEGALCOPYRIGHT_STR=\"Copyright (C) 2004-2023 Frank Becker\";
            VER_COMPANYNAME_STR=\"mooflu.com\";
            VER_FILEDESCRIPTION_STR=\"3D block stacking game\";
            VER_FILEVERSION_STR=\"${VERSION}\";
            VER_PRODUCTNAME_STR=\"Critter\";
            VER_PRODUCTVERSION_STR=\"${VERSION}\";
            VER_FILEVERSION=${VERSION_COMMA},0;
            VER_PRODUCTVERSION=${VERSION_COMMA},0;
        "
    )
    set(EXTRA_LIBRARIES
        imm32.lib
        winmm.lib
        version.lib
    )
endif()
if(APPLE)
    set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/critter.icns)
    set_source_files_properties(${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    set_source_files_properties(${RESOURCE_DAT} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    FIND_LIBRARY(FF_LIBRARY ForceFeedback)
    FIND_LIBRARY(IOKIT_LIBRARY IOKit)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    FIND_LIBRARY(COREVIDEO_LIBRARY CoreVideo)
    FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    FIND_LIBRARY(AUDIOUNIT_LIBRARY AudioUnit)
    FIND_LIBRARY(ICONV_LIBRARY iconv)

    set(EXTRA_LIBRARIES
        ${CARBON_LIBRARY}
        ${FF_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREAUDIO_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${AUDIOUNIT_LIBRARY}
        ${ICONV_LIBRARY}
    )
endif()

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/utils)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/utilsfs)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/utilsgl)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/utilssdl)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/utilsrandom)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/tinyxml)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/miniyaml)
include_directories(${PROJECT_SOURCE_DIR}/mooflu.common/) # vmmlib/vector.hpp

file(GLOB GAME_SRC *.cpp)
file(GLOB GAME_HEADERS *.hpp)

add_executable(critter WIN32 MACOSX_BUNDLE ${GAME_SRC} ${GAME_HEADERS} ${APP_ICON} ${RESOURCE_DAT})

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT critter)

if(EMSCRIPTEN)
    message(STATUS "Setting compilation target to EMSCRIPTEN")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    string(REGEX REPLACE "\n *" " " EMSCRIPTEN_LINK_FLAGS "
        -Oz
        -s USE_SDL=2
        -s USE_SDL_IMAGE=2
        -s SDL2_IMAGE_FORMATS='[\"png\"]'
        -s USE_ZLIB=1
        -s USE_LIBPNG=1
        -s USE_ICU=1
        -s ALLOW_MEMORY_GROWTH=1
        -s EXPORTED_FUNCTIONS='[_main]'
        -s EXPORTED_RUNTIME_METHODS=['UTF8ToString']
        --preload-file ${CMAKE_SOURCE_DIR}/resource.dat@resource.dat
        -lidbfs.js
    ")
# -flto
# -s BINARYEN_METHOD='native-wasm'
# --emrun
# -O2
# --llvm-lto 1

#-s USE_OGG=1
#-s USE_VORBIS=1
#        --preload-file ${CMAKE_SOURCE_DIR}/data@data
#        --closure 1
#        -s FULL_ES3=1
#        -s LEGACY_GL_EMULATION=1

    set_target_properties(critter PROPERTIES LINK_FLAGS ${EMSCRIPTEN_LINK_FLAGS})
endif()

if(NOT EMSCRIPTEN)
    set(EXTRA_LIBRARIES_2
        ${SDL2_MIXER_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${PHYSFS_LIBRARY}
    )
    if(WIN32)
        set(EXTRA_LIBRARIES_2 ${EXTRA_LIBRARIES_2} GLEW::glew_s)
    else()
        if (BUILD_SHARED_LIBS)
            set(EXTRA_LIBRARIES_2 ${EXTRA_LIBRARIES_2} ${GLEW_LIBRARIES})
        else()
            set(EXTRA_LIBRARIES_2 ${EXTRA_LIBRARIES_2} GLEW::glew_s)
        endif()
    endif()
else()
    set(EXTRA_LIBRARIES_2
        ${SDL2_MIXER_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        "${CMAKE_INSTALL_PREFIX}/lib/libphysfs.a"
    )
endif()

target_link_libraries(critter utils utilsrandom utilssdl utilsfs utilsgl tinyxml miniyaml
${SDL2_LIBRARIES}
${PNG_LIBRARY}
${ZLIB_LIBRARY}
${OPENGL_LIBRARY}
${EXTRA_LIBRARIES}
${EXTRA_LIBRARIES_2}
${CMAKE_DL_LIBS}
)
