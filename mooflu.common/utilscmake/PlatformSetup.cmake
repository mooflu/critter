MESSAGE( "Running platform setup..." )

if(WIN32)
    if(NOT DEFINED CMAKE_PREFIX_PATH)
        MESSAGE( WARNING "CMAKE_PREFIX_PATH not set. Setting to C:\\sandbox" )
        LIST(APPEND CMAKE_PREFIX_PATH "C:\\sandbox")
    endif()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    #MESSAGE(${CMAKE_CXX_FLAGS})
endif()

#gcc check has to come *after* setting project
if(CMAKE_COMPILER_IS_GNUCXX)
  exec_program(
      ${CMAKE_CXX_COMPILER}
      ARGS                    --version
      OUTPUT_VARIABLE _compiler_output)
  string(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]).*" "\\1"
         gcc_compiler_version ${_compiler_output})
  message(STATUS "GCC compiler version: ${gcc_compiler_version}")

  if(gcc_compiler_version VERSION_GREATER "4.2.99")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") # -pedantic -Wall -Wextra -Weffc++")
  endif()
else()
  if(WIN32)
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Weffc++") # -pedantic -Wall -Wextra -Weffc++")
  endif()
endif(CMAKE_COMPILER_IS_GNUCXX)

