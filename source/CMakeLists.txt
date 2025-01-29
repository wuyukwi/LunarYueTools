include(macros)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VS_TARGET_NAME})
endif()

print_info()
set_output_paths()

add_subdirectory_ex(3rdparty)
add_subdirectory_ex(core)
add_subdirectory_ex(application)
