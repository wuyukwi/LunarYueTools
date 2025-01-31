cmake_minimum_required(VERSION 3.10)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)
set(LIB_NAME imgui)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
)

set(IMGUI_INCLUDES
    ${IMGUI_DIR}
)

option(IMGUI_BACKEND_SDL2 "Enable SDL2 backend" OFF)
option(IMGUI_BACKEND_SDL3 "Enable SDL3 backend" ON)
option(IMGUI_BACKEND_GLFW "Enable GLFW backend" OFF)
option(IMGUI_BACKEND_VULKAN "Enable Vulkan backend" ON)
option(IMGUI_BACKEND_DX11 "Enable DirectX 11 backend" ON)
option(IMGUI_BACKEND_DX12 "Enable DirectX 12 backend" ON)
option(IMGUI_BACKEND_WGPU "Enable WebGPU backend" OFF)
option(IMGUI_BACKEND_WIN32 "Enable Win32 backend" ON)

if (IMGUI_BACKEND_SDL2)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl2.h
    )
endif()

if (IMGUI_BACKEND_SDL3)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl3.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl3.h
    )
endif()

if (IMGUI_BACKEND_GLFW)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.h
    )
endif()

if (IMGUI_BACKEND_VULKAN)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_vulkan.h
    )
endif()

if (IMGUI_BACKEND_DX11)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_dx11.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_dx11.h
    )
endif()

if (IMGUI_BACKEND_DX12)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_dx12.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_dx12.h
    )
endif()

if (IMGUI_BACKEND_WGPU)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_wgpu.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_wgpu.h
    )
endif()

if (IMGUI_BACKEND_WIN32)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_win32.h
    )
endif()

add_library(${LIB_NAME} STATIC ${IMGUI_SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${IMGUI_INCLUDES} ${IMGUI_BACKENDS_DIR})

if (IMGUI_BACKEND_SDL3)
    target_link_libraries(${LIB_NAME} PUBLIC SDL3::SDL3)
endif()

if (IMGUI_BACKEND_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${LIB_NAME} PUBLIC Vulkan::Vulkan)
endif()

if (IMGUI_BACKEND_DX11)
    target_link_libraries(${LIB_NAME} PUBLIC dxgi)
endif()

if (IMGUI_BACKEND_DX12)
    target_link_libraries(${LIB_NAME} PUBLIC dxgi d3d12)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${IMGUI_SOURCES})

set_target_properties(${LIB_NAME} PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
