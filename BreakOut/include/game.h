
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gameLevel.h"
#include "ballObject.h"
#include "light.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// 撞击方向
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

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
    void Render(float dt);
    // collision
    void DoCollisions();
    //GLboolean CheckCollision(GameObject &one, GameObject &two);
    //GLboolean CheckCollision(BallObject &one, GameObject &two);
    Collision CheckCollision(BallObject &one, GameObject &two);
    float clamp(float value, float min, float max);
    Direction VectorDirection(glm::vec2 target);

    // reset
    void ResetLevel();
    void ResetPlayer();

    int Lives;

    // 防止按一下快速跳跃
    GLboolean KeysProcessed[1024];

    // 分数
    int Score;

};

#endif