#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel{
    public:

    std::vector<GameObject> Bricks;

    GameLevel() { }
    // loads level from file
    void Load(const char *file, unsigned int levelWidth, unsigned int levelHeight);

    // render level
    void Draw(SpriteRenderer &renderer);

    // check if the level is completed (all non-solid titles are destroyed)
    bool IsCompleted();

    std::vector<std::vector<unsigned int>> tileData;

private:
    // initialize level from title data
    void init(std::vector<std::vector<unsigned int>> tileData,
              unsigned int levelWidth, unsigned int levelHeight);
};

#endif