set(CORE_FOLDER Core)
set(LIB_NAME Core)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (${LIB_NAME} ${libsrc})

set_target_properties(${LIB_NAME} PROPERTIES FOLDER ${CORE_FOLDER})

target_link_libraries(${LIB_NAME} PUBLIC imgui)
target_link_libraries(${LIB_NAME} PUBLIC spdlog)

target_include_directories (${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
