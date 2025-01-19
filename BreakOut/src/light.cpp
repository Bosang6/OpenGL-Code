#include "light.h"

Light::Light() 
    : Position(glm::vec3(0.0f, 0.0f, 0.0f)), Color(glm::vec3(0.0f, 0.0f, 0.0f)) { }

Light::Light(glm::vec3 pos, glm::vec3 color)
    : Position(pos), Color(color) { }