
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gameLevel.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// paddle size
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    std::vector<GameLevel> Levels;
    GLuint Level;
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    // collision
    void DoCollisions();
    GLboolean CheckCollision(GameObject &one, GameObject &two);
};

#endif