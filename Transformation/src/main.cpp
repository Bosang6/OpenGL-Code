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






    /* 学习区域 */

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);

    /*
        在0.9.9以上的版本
        glm::mat4 trans = glm::mat4(1.0f);
    */

    // 建立一个 4 * 4的单位矩阵
    glm::mat4 trans = glm::mat4(1.0f);

    /*
        1 0 0 0        1 0 0 1
        0 1 0 0        0 1 0 1
        0 0 1 0  -->   0 0 1 0
        0 0 0 1        0 0 0 1
                             ^
                             |
                         添加了位移
    */
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));

    // 以矩阵（4*4）乘向量形式完成转换
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;


    /* 学习区域 */


    Shader shader("shader/shader.vs", "shader/shader.fs");


    /* --几何定义-- */

    // 长方形的左边定义
    float vertices[]{
    //      位置                 纹理坐标
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f            
    };

    unsigned int index[]{
        //0,1,2,
        //0,3,2
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    /* --数据加载-- */

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
 



    /* --纹理加载-- */

    /*
    纹理加载顺序：
        - 创建 int变量 作为指针
        - 通过 glGenTextures() 来确定将要在显存开辟几个用于存储纹理的空间
        - 通过 glBindTexture() 赋予给int变量 纹理将要保存的为显存地址
        - 绑定一些列的纹理数据
        - 从文件夹内加载所需要的纹理（图片）
        - 加载完毕后清除图片在内存中的缓存（因为已经被加载进显存）stbi_image_free(data)
    */

    // 在RAM中 创建OpenGL的 texture指针
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 读取图片数据
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("texture/container.jpg", &width, &height, &nrChannels, 0);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,  0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    // 加载第二张纹理

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("texture/awesomeface.png", &width, &height, &nrChannels, 0);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    shader.use();
    // glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);


    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // 背景颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 启用OpenGL的纹理模式
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        /* --旋转定义-- */

        glm::mat4 transR = glm::mat4(1.0f);
        // 沿着z轴 逆时针旋转90度
        // transR = glm::rotate(transR, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

        // 沿着z轴 随着时间旋转
        transR = glm::rotate(transR, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

        transR = glm::scale(transR, glm::vec3(0.5, 0.5, 0.5)); 

        glm::mat4 transformation = glm::mat4(1.0f);
        transformation = glm::translate(transformation, glm::vec3(0.5, -0.5, 0.0));

        // transR = glm::translate(transR, glm::vec3(0.5, -0.5, 0.0));

        shader.use();
        // 寻找着色器内的 Uniform Variabile位置
        unsigned int transRLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transRLoc, 1, GL_FALSE, glm::value_ptr(transR));
        /*                     ^         ^      ^           ^
                            |         |      |           |
                            位置     几个矩阵 是否转置     矩阵数据
        */
        unsigned int transformationLoc = glGetUniformLocation(shader.ID, "transformation");
        glUniformMatrix4fv(transformationLoc, 1, GL_FALSE, glm::value_ptr(transformation));
        
        // 启用着色器并重新绑定VAO
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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