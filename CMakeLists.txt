cmake_minimum_required(VERSION 3.16)
project(3DProdGen)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/raylib)
add_subdirectory(external/glm)

add_executable(3DProdGen main.cpp
        src/bStateTemplate.hpp
        src/Program.cpp
        src/Program.hpp
        src/ShaderManager.cpp
        src/ShaderManager.hpp
        src/MainMenu.cpp
        src/MainMenu.hpp)

target_link_libraries(3DProdGen raylib)
target_link_libraries(3DProdGen glm)

if(UNIX)
    target_link_libraries(3DProdGen m pthread dl X11 Xrandr Xi Xxf86vm Xinerama Xcursor GL)
endif()
