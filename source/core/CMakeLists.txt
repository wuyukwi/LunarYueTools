set(CORE_FOLDER Core)
set(LIB_NAME Core)

set(libsrc
    src/cmd_line/parser.hpp

    src/event/EventManager.h
    src/event/EventSubscriber.h
    src/event/frame_event.h
    src/event/sdl_event.h

    src/gui/gui.cpp
    src/gui/gui.h

    src/renderer/vulkan/vulkan_renderer.cpp
    src/renderer/vulkan/vulkan_renderer.h
    src/renderer/renderer.cpp
    src/renderer/renderer.h

    src/system/subsystem.cpp
    src/system/subsystem.h

    src/window/window.cpp
    src/window/window.h
    src/window/window_manager.cpp
    src/window/window_manager.h

    src/app.cpp
    src/app.h
    src/core.cpp
    src/core.h
    src/logger.cpp
    src/logger.h
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library(${LIB_NAME} ${libsrc})

set_target_properties(${LIB_NAME} PROPERTIES FOLDER ${CORE_FOLDER})
target_link_libraries(${LIB_NAME}
    PUBLIC
        imgui
        spdlog
)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
