project(UTILS)

option(USE_ZEROCONF "Enable ZeroConf (requires dns_sd.h)" off)

if(NOT EMSCRIPTEN)
    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

include_directories(${CMAKE_PREFIX_PATH}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(UTILS_SRC
Endian.cpp
FPS.cpp
Polynomial.cpp
RectanglePacker.cpp
sha2.c
Tea.cpp
Trace.cpp
Value.cpp
)

file(GLOB UTILS_HEADERS *.h *.hpp)

if(USE_ZEROCONF)
  list(APPEND UTILS_SRC ZeroConf.cpp)
endif(USE_ZEROCONF)

add_library(utils ${UTILS_SRC} ${UTILS_HEADERS})

install(FILES ${UTILS_HEADERS} DESTINATION include/utils)
install(TARGETS utils ARCHIVE DESTINATION lib)
