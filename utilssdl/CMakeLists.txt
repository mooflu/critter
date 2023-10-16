project(UTILSSDL)

if(NOT EMSCRIPTEN)
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIR})
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
endif()

include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utilsfs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../miniyaml)

file(GLOB UTILSSDL_SRC *.cpp)
file(GLOB UTILSSDL_HEADERS *.hpp)

add_library(utilssdl ${UTILSSDL_SRC} ${UTILSSDL_HEADERS})

install(FILES ${UTILSSDL_HEADERS} DESTINATION include/utilssdl)
install(TARGETS utilssdl ARCHIVE DESTINATION lib)
