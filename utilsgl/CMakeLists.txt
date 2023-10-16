project(UTILSGL)

if(NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utilsfs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utilssdl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

file(GLOB UTILSGL_SRC *.cpp gl3/*.cpp)
file(GLOB UTILSGL_HEADERS *.hpp gl3/*.hpp)

add_library(utilsgl ${UTILSGL_SRC} ${UTILSGL_HEADERS})
if (NOT BUILD_SHARED_LIBS)
    set_target_properties (utilsgl PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC")
endif()

install(FILES ${UTILSGL_HEADERS} DESTINATION include/utilsgl)
install(TARGETS utilsgl ARCHIVE DESTINATION lib)
