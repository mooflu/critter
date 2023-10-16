project(UTILSRANDOM)

include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB UTILSRANDOM_SRC *.cpp)
file(GLOB UTILSRANDOM_HEADERS *.h *.hpp)

add_library(utilsrandom ${UTILSRANDOM_SRC} ${UTILSRANDOM_HEADERS})

install(FILES ${UTILSRANDOM_HEADERS} DESTINATION include/utilsrandom)
install(TARGETS utilsrandom ARCHIVE DESTINATION lib)
