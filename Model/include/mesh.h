
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string.h>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type; 
};

class Mesh {
    public:
        /* 网格数据 */
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /* 函数 */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader shader);
    private:
        /* 渲染数据 */
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

Mesh(vec)