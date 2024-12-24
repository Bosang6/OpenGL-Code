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


// 
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// 固定窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 相机坐标定义
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastFrame = 0.0f;
float deltaTime = 0.0f;

// 鼠标
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

// 重力 + 跳跃
float cameraYVelocity = 0.0f; // 垂直速度
float gravity = -9.8f;
float jumpStrength = 5.0f; // 初始跳跃速度
bool isJumping = false;

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

    // 添加鼠标的回调函数
    glfwSetCursorPosCallback(window, mouse_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("shader/shader.vs", "shader/shader.fs");

    Shader lampShader("shader/shader.vs", "shader/lampShader.fs");



    /* --几何定义-- */

    // 正方体定义

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    glm::vec3 cubePosition = glm::vec3(0.0f, 0.0f, 0.0f);


    /* --数据加载-- */

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 光源位置

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);


    // 使用箱子的结构（正方体）作为光源
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
 

    glEnable(GL_DEPTH_TEST);


    while(!glfwWindowShouldClose(window)){
        // 计算一帧渲染的时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        cameraYVelocity += gravity * deltaTime; // v = v0 + a * t
                                                // 初始速度会随着 重力的影响逐渐减小
                                                // 当 初始速度 + 重力影响的速度 小于0时，则说明相机开始向下移动
        cameraPos.y += cameraYVelocity * deltaTime; // 随着绘制时间帧移动

        // 当相机落地时，将相机跳跃的初始速度设为0
        if(cameraPos.y <= 0.0f){
            cameraPos.y = 0.0f;
            cameraYVelocity = 0.0f;
            isJumping = false;
        }

        // 背景颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::vec3 boxColor = glm::vec3(1.0f, 0.5f, 0.31f);
        shader.setVec3("objectColor", boxColor);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // 世界坐标系与相机坐标系的变换
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 投影矩阵, 用于透视效果
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        shader.use();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glm::mat4 model(1.0f);
        model = glm::translate(model, cubePosition);
        shader.setMat4("model",model);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        lampShader.use();

        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);



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
    
    float y = cameraPos.y;

    // 走路速度
    float cameraSpeed = 0.05f;

    // 跳跃速度
    float cameraJump = 0.5f;
    
    /* 由于硬件的不同，processInput函数可能被调用的次数不同。 
       为了使在所有设备上的移动速度统一，我们引入 deltaTime, 来计算每一帧渲染前后的时间差
    */
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += cameraSpeed * cameraFront;
        cameraPos.y = y;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= cameraSpeed * cameraFront;
        cameraPos.y = y;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping){
        isJumping = true;
        cameraYVelocity = jumpStrength; // 在按下空格时，为相机设置一个初始速度
    

        // cameraPos.y = min(cameraPos.y + cameraJump * cameraUp.y - 9.8f * , 0);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        cameraPos -= cameraJump * cameraUp;
    }
}

// 更改窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos -lastX;
    // Y轴的正方向在下
    float yoffset = lastY - ypos;
    // float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    // 我们并没有现实意义上的计算出了pitch和yaw的真实角度，而是通过offset来模拟角度
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f){
        pitch = 89.0f;
    }
    if(pitch < -89.0f){
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}
