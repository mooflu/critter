find_package(SWIG REQUIRED)

# Add swig interface file to a list of generated files.
# E.g.: Generate lua c++ wrappers for interface.hpp and add to GEN_SRC
#       swig_add_interface( interface.hpp GEN_SRC lua "-c++")

macro(SWIG_ADD_INTERFACE interface_file generated_files language)

    set(flags "${ARGV3}")

    get_filename_component(base_filename "${interface_file}" NAME_WE)

    set(generated_fullname "${CMAKE_CURRENT_BINARY_DIR}/${base_filename}_swig.cpp")
    set(interface_fullname "${CMAKE_CURRENT_SOURCE_DIR}/${interface_file}")

    add_custom_command(
        OUTPUT "${generated_fullname}"
        COMMAND "${SWIG_EXECUTABLE}"
        ARGS "-${language}" ${flags} -o "${generated_fullname}" "${interface_fullname}"
        MAIN_DEPENDENCY "${interface_fullname}"
        COMMENT "Generating ${language} wrapper for ${interface_file}"
    )

    set(${generated_files} ${${generated_files}} "${generated_fullname}")

endmacro(SWIG_ADD_INTERFACE)

