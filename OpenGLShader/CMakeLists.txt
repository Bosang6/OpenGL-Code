cmake_minimum_required(VERSION 3.10)
project(MyOpenGLProject)

# 设置 C++ 版本
set(CMAKE_CXX_STANDARD 11)

# 包含 GLFW 和 GLAD 的头文件路径
include_directories(include)

# 查找 GLFW 库
find_package(glfw3 3.3 REQUIRED)

# 源文件
set(SOURCES glad.c src/retangolo.cpp)

# 将可执行文件的输出路径设置为项目根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} glfw "-framework OpenGL")
