set(APP_NAME test)

file(GLOB_RECURSE libsrc "*.h" "*.cpp" "*.hpp" "*.c" "*.cc")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_executable (${APP_NAME} ${libsrc} ${EDITOR_RESOURCE})

target_link_libraries(${APP_NAME} PUBLIC Core)

set_target_properties(${APP_NAME} PROPERTIES FOLDER ${APP_FOLDER})