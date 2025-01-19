#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>


class Light{
    public:
        glm::vec3 Position, Color;
        
        Light();
        Light(glm::vec3 pos, glm::vec3 color);
};

#endif