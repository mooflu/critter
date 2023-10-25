project(UTILSFS)

if(NOT EMSCRIPTEN)
    find_package(PhysFS REQUIRED)
    include_directories(${PHYSFS_INCLUDE_DIR})
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
  endif()

include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../miniyaml)

set(UTILSFS_SRC
zStreamBuffer.cpp
zStream.cpp
Config.cpp
ResourceManager.cpp
Translator.cpp
WalkDirectory.cpp
)

if(APPLE)
  list(APPEND UTILSFS_SRC GetDataPath.mm)
else()
  list(APPEND UTILSFS_SRC GetDataPath.cpp)
endif()

if( UTILSSDL_SOURCE_DIR )
  include_directories(${SDL_INCLUDE_DIR})
  list(APPEND UTILSFS_SRC zrwops.cpp)
endif()

file(GLOB UTILSFS_HEADERS *.hpp)

add_library(utilsfs ${UTILSFS_SRC} ${UTILSFS_HEADERS})

install(FILES ${UTILSFS_HEADERS} DESTINATION include/utilsfs)
install(TARGETS utilsfs ARCHIVE DESTINATION lib)
