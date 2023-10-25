project(MINIYAML)

set(MINIYAML_SRC
Yaml.cpp
)

file(GLOB MINIYAML_HEADERS *.hpp)

add_library(miniyaml ${MINIYAML_SRC} ${MINIYAML_HEADERS})

install(FILES ${MINIYAML_HEADERS} DESTINATION include/miniyaml)
install(TARGETS miniyaml ARCHIVE DESTINATION lib)
