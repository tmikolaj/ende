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
        external/imgui/ImGuiFileDialog.cpp
        external/imgui/ImGuiFileDialog.h
        external/imgui/ImGuiFileDialogConfig.h
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
        src/app/Application.cpp
        src/app/StateController.cpp
        src/app/main.cpp

        src/core/Context.hpp
        src/core/CustomCamera.cpp
        src/core/CustomCamera.hpp

        src/debug/NormalVisualizer.cpp
        src/debug/NormalVisualizer.hpp
        src/debug/UVChecker.cpp
        src/debug/UVChecker.hpp

        src/entities/Entity.hpp
        src/entities/CustomMeshFactory.cpp
        src/entities/CustomMeshFactory.hpp
        src/entities/RockEntity.cpp
        src/entities/RockEntity.hpp
        src/entities/TerrainEntity.cpp
        src/entities/TerrainEntity.hpp

        src/graphics/ShaderManager.cpp
        src/graphics/ShaderManager.hpp
        src/graphics/lLight.hpp

        src/math/Noise.cpp
        src/math/Noise.hpp

        src/shapers/Shaper.hpp
        src/shapers/SubdivisionShaper.cpp
        src/shapers/SubdivisionShaper.hpp

        src/states/BaseState.hpp
        src/states/EntityPaintState.cpp
        src/states/EntityPaintState.hpp
        src/states/SceneEditorState.cpp
        src/states/SceneEditorState.hpp
        src/states/StartMenuState.cpp
        src/states/SimulationState.cpp
        src/states/SimulationState.hpp

        src/ui/AppUI.cpp
        src/ui/AppUI.hpp
        src/ui/FontManager.cpp
        src/ui/FontManager.hpp
        src/ui/UIManager.cpp
        src/ui/UIManager.hpp

        src/physics/ThermalErosion.cpp
        src/physics/ThermalErosion.hpp
        src/ui/StyleManager.cpp
        src/ui/StyleManager.hpp

        src/Exporter.cpp
        src/Exporter.hpp
)

target_link_libraries(3DProdGen
        raylib
        glm
        rlImGui
)

if (WIN32)
    enable_language(RC)
    set(WINDOWS_ICON_RC ${PROJECT_SOURCE_DIR}/assets/icons/endeicon.ico)
    target_sources(3DProdGen PRIVATE ${WINDOWS_ICON_RC})
endif()

if (UNIX)
    target_link_libraries(3DProdGen m pthread dl X11 Xrandr Xi Xxf86vm Xinerama Xcursor GL)
endif()
