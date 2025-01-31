set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(macros)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VS_TARGET_NAME})
endif()

set_output_paths()

add_subdirectory_ex(3rdparty)
add_subdirectory_ex(core)
add_subdirectory_ex(application)
