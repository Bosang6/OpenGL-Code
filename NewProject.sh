#!/bin/bash

# 检查是否提供了正确数量的参数
if [ $# -ne 1 ]; then
  echo "参数错误！请提供一个项目名称作为参数。例如："
  echo "./create_opengl_project.sh MyOpenGLProject"
  exit 1
fi

# 获取项目名称
project_name=$1

# 创建项目文件夹
mkdir "$project_name"

# 检查文件夹是否创建成功
if [ $? -ne 0 ]; then
  echo "创建文件夹失败，可能是文件夹已存在。"
  exit 1
fi

echo "创建项目文件夹: $project_name"


# 获取脚本所在目录
script_dir=$(dirname "$0")

# 从脚本所在目录下的 OpenGLIncludes 文件夹复制文件
opengl_includes="$script_dir/OpenGLIncludes"
if [ -d "$opengl_includes" ]; then
  cp -r "$opengl_includes"/* "$project_name/"
  echo "从 $opengl_includes 复制文件到 $project_name/"
else
  echo "未找到与脚本同目录下的 OpenGLIncludes 文件夹，跳过复制。"
fi

# 进入项目文件夹
cd "$project_name"

# 创建子文件夹
mkdir build src texture shader
echo "创建子文件夹: build, src, texture, shader"

# 在 src 文件夹中创建 main.cpp 文件
# touch src/main.cpp

cat <<EOF > src/main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_s.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
//
void processInput(GLFWwindow *window);

// 固定窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(){
    // OpenGL 3.3版本的core模式初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // MAC系统解释器
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口并且验证是否开启成功
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader shader("shader/shader.vs", "shader/shader.fs");

    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // 背景颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 启用着色器并重新绑定VAO


        // Double Buffer用于动画
        glfwSwapBuffers(window);
        // 获取用户操作
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

// 窗口内的输入获取
void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

// 更改窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}
EOF

echo "创建 src/main.cpp 文件"

echo "OpenGL 项目文件夹已成功创建！"
