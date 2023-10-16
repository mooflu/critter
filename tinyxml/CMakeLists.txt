project(TINYXML)

set(TINYXML_SRC
tinyxml.cpp
tinyxmlerror.cpp
tinyxmlparser.cpp
)

file(GLOB TINYXML_HEADERS *.h)

add_library(tinyxml ${TINYXML_SRC} ${TINYXML_HEADERS})

install(FILES ${TINYXML_HEADERS} DESTINATION include/tinyxml)
install(TARGETS tinyxml ARCHIVE DESTINATION lib)
