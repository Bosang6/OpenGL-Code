#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    float vertice[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    unsigned int VBO; // VBO 表示在GPU显存上的一段存储空间对象，类似ID编号
    glGenBuffers(1, &VBO); // 一个指向GPU显存的对象指针

    /*
    
    VBO的创建和销毁

    创建：void glGenBuffers(GLsize n, Gluint *buffers) 显存的一组描述信息，还未被分配到真正的显存

    n : 创建VBO的数量
    buffers : 保存VBO的编号，可以是多个

    GLunit vboArr[] = {0, 0, 0};
    glGenBuffers(3,vboArr);


    VBO的销毁

    void glDeletBuffers(GLsize n, GLuint *buffers)

    n: 删除VBO的数量
    buffer：需要被删除的VBO的编号们

    */

    /*
     VBO绑定OpenGL状态机
     
     void glBindBuffer(GLenum target， GLunit buffer);

     target: 把当前VBO绑定到状态机的某个插槽

     buffer: 绑定的VBO编号

    */

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //GL_ARRAY_BUFFER 为OpenGL的自动机的一个状态，即用于保存VBO

    /*
    往VBO内写入数据, 复制到显存

    void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage)

    target: OpenGL自动机插槽

    size: 写入的数据内容大小，一般用sizeof()函数

    data: 写入数据的指针

    usage: 当前buffer的用法
        - GL_STATIC_DRAW 静态绘制，VBO的数据不会频繁的改变
        - GL_DYNAMIC_DRAW 动态绘制，VBO的的数据会频繁改变
        - GL_STREAM_DRAW 流绘制，每次绘制都会改变

    注意事项: 该函数调用的使用会重新开辟空间，频繁使用会导致严重卡顿，谨慎使用
    */

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);



    /*
    在现代OpenGL中，要求代码中必须包含一个vertex shader和一个fragment shader
    */

   // 以字符串的形式来编写vertex shader, 也可以以文本的形式定义
   const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl Position = vec4(a.Pos.x, a.Pos.y, a.Pos.z, 1.0);\n"
        "}\0";


    // 我们需要在main中动态的编译着色器， 用一个ID（unsigned int）来保存，用于后续在显存中创建

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    /*
    glShaderSource(着色器对象（ID）， 字符串数量， 着色器代码（字符串）， NULL)；
     */

    // 将手写的 vertex shader代码附给ID，然后编译
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    /*
    判断是否编译成功：
    - 创建一个int变量 int success
    - 用一个 char数组保存编译后的结果输出（字符串）
    - 通过glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success)函数，为success保存编译的结果
     */

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    /*
    如果编译失败，则从着色器ID中获取错误信息，并保存到infoLog数组内
     */
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /*
    在编译完vertex shader后，再编译fragment shader编译。
    */

   const char *fragmentShaderSource = "#version 330 core\n"
   "out vec4 FragColor\n"
   "void main()\n"
   "{\n"
   "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
   "}\n";

   /*
   与vertex shader一致，通过ID在显存中保存fragment shader的信息
   */

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  /*
  Shader Program Object LINK
  在编写完所需的shader后，我们需要将这些shader绑定在一起,即我们的着色器程序
  - 创建一个shader program 的ID
  - 通过glCreateProgram(void)函数来绑定ID
  - 通过glAttachShader(ID, shader)函数，将我们编写的shader绑定在shader program的ID
  - 最后通过glLinkProgram(ID)完成链接
  */

 unsigned int shaderProgram;
 shaderProgram = glCreateProgram();

 glAttachShader(shaderProgram, vertexShader);
 glAttachShader(shaderProgram, fragmentShader);
 glLinkProgram(shaderProgram);

 /*
 同理，通过success变量来检查是否链接成功

 - glGetProgramiv()函数将链接是否成功书写到success变量上
 - 通过glGetProgramInfoLog()函数获取错误信息并打印出来
 */

glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if(!success){
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}

/*
完成着色器链接后，我们可以通过 glUseProgramm(shaderProgramm)函数来调用该程序
*/

// glUseProgram(shaderProgram);

/*
着色器程序完成链接后，先前在显存中创建的VBO（vertex shader 和 fragment shader）就不再有用
可以通过 glDeleteShader(shader)来释放显存空间
*/

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

/*
此后，显存中已经存在了我们编写的各种信息，但我们还需要告知OpenGL如何解释这些数据

在传入的vertex数组中，并不是只有xyz的三维空间坐标，可以通过插值的方式为对应的坐标赋予不同的信息（颜色等）
因此，我们需要解释数组中的内容

数组中，每个元素的长度为 32bit（4byte），通常，一组信息的前三个元素为三维空间的坐标

glVertexAttribPointer()函数告诉OpenGL怎么解析顶点数据
参数：
    - 匹配着色器内的 location = （int)
    - 一个顶点需要至少三个值来定义空间位置，因此为3
    - GL_FLOAT 说明显存内的数据为float类型
    - GL_TRUE / GL_FALSE 是否需要标准化 [-1, 1]
    - 数据步长，下一组数据的位置。 可以写0让OpenGL自行判断，但必须要求是连续的空间坐标。
    - void*  强制类型转换，表示数据在缓冲buffer内的偏移量

OpenGL得知数据结构后，通过glEnableVertexAttribArray(index location)来启用, 默认是禁止的

告知OpenGL怎么理解数据以及启用后，才方可使用glUseProgram(shaderProgram)来使用着色器
 */

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);

glUseProgram(shaderProgram);



/*
每次绘制一个物体都必须重复以上过程，当我们绘制多个物体时，重复以上的操作显得特别冗余复杂
为此，引入了VAO(Vertex Array Object):
    VAO是VBO的数据使用指南，我们应该告诉OpenGL哪些数据是物体的结构，哪些数据是颜色等
    在 3.0 CORE模式下，OpenGL只会通过VAO来使用VBO的数据，若不通过VAO来告知OpenGL怎么使用数据，OpenGL将不会绘画任何东西

    VAO调用的函数：
    - glEnableVertexAttribArray()
    - glDisableVertexAttribArray() 
    - glVertexAttribPointer() 在一个VAO数组中，每个元素为一个指向VBO的指针
*/

/*
创建一个VAO
*/

unsigned int VAO;
glGenVertexArrays(1, &VAO); // 创建一个VAO，并告知VAO的数量。 可以是数组


    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件