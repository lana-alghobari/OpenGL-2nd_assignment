cmake_minimum_required(VERSION 3.10)
project(SolarSystem)
set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR "lib/glfw64")
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib-mingw-w64)

set(GLEW_DIR "lib/glew")
include_directories(${GLEW_DIR}/include)
link_directories(${GLEW_DIR}/lib/Release/x64)

set(GLM "lib/glm")
include_directories(${GLM})

set(ASSIMP "lib/Assimp")
include_directories(${ASSIMP}/include)
link_directories(${ASSIMP}/lib/x64)

set(imgui "lib/imgui")
include_directories(${imgui})



add_executable(SolarSystem main.cpp)


target_link_libraries(SolarSystem glfw3 glew32 opengl32 libassimp)
