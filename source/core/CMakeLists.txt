set(CORE_FOLDER Core)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (Core ${libsrc})

set_target_properties(Core PROPERTIES FOLDER ${CORE_FOLDER})

target_include_directories (Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
