cmake_minimum_required(VERSION 3.16)
project(3DProdGen)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/raylib)
add_subdirectory(external/glm)

set(IMGUI_SRC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC external/imgui)

add_library(rlImGui external/rlImGui/rlImGui.cpp)
target_include_directories(rlImGui PUBLIC
        external/rlImGui
        ${PROJECT_SOURCE_DIR}/external/imgui
        ${PROJECT_SOURCE_DIR}/external/raylib/src
)
target_link_libraries(rlImGui PUBLIC imgui raylib)

add_executable(3DProdGen
        main.cpp
        src/Program.cpp
        src/ShaderManager.cpp
        src/StartMenu.cpp
        src/StateManager.cpp
)

target_link_libraries(3DProdGen
        raylib
        glm
        rlImGui
)

if(UNIX)
    target_link_libraries(3DProdGen m pthread dl X11 Xrandr Xi Xxf86vm Xinerama Xcursor GL)
endif()
