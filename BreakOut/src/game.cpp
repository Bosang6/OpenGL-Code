#include "gameLevel.h"
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ballObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

const float BALL_RADIUS = 12.5f;

BallObject *Ball;

SpriteRenderer *Renderer;
GameObject *Player;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Player;
}


void Game::Init()
{
    ResourceManager::LoadShader("shader/spriteVertex.vs", "shader/spriteFragment.fs", nullptr, "sprite");

    Shader &spriteShader = ResourceManager::GetShader("sprite");

    //glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
    /*
    正交函数定义了 left，right，top，down四条边的界限
    (0,0)    ...   (800,0)
    .                  .
    .                  .
    .                  .
    (0,600)  ...   (800,600)
    */

    spriteShader.Use().SetInteger("image", 0);
    spriteShader.SetMatrix4("projection", projection);

    //ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    //ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(spriteShader);
    //Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture("texture/background.jpg", false, "background");
    ResourceManager::LoadTexture("texture/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("texture/block.png", false, "block");
    ResourceManager::LoadTexture("texture/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("texture/paddle.png", true, "paddle");

    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    // load levels

    GameLevel test; test.Load("level/levelTest.txt", this->Width, this->Height / 2);

    GameLevel one; one.Load("level/level1.txt", this->Width, this->Height / 2);
    GameLevel two; two.Load("level/level2.txt", this->Width, this->Height / 2);
    GameLevel three; three.Load("level/level3.txt", this->Width, this->Height / 2);
    GameLevel four; four.Load("level/level4.txt", this->Width, this->Height / 2);

    this->Levels.push_back(test);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 2;
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();

    if(Ball->Position.y >= this->Height){ // 球体是否接触到底部边界
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::ProcessInput(float dt)
{
    if(this->State == GAME_ACTIVE){
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if(this->Keys[GLFW_KEY_A]){
            if(Player->Position.x >= 0.0f){
                Player->Position.x -= velocity;
                if(Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if(this->Keys[GLFW_KEY_D]){
            if(Player->Position.x <= this->Width - Player->Size.x){
                Player->Position.x += velocity;
                if(Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if(this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE){
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level

        this->Levels[this->Level].Draw(*Renderer);

        Ball->Draw(*Renderer);
    }

    Player->Draw(*Renderer);
}

void Game::DoCollisions(){
    for(GameObject &box : this->Levels[this->Level].Bricks){
        if(!box.Destroyed){

            // 穿模碰撞
            /*
            if(CheckCollision(*Ball, box)){
                if(!box.IsSolid)
                    box.Destroyed = GL_TRUE;
            }
            */

           Collision collision = CheckCollision(*Ball, box);
           if(std::get<0>(collision)){ // 如果 检测到碰撞
                // 如果砖块不是实心，则销毁
                if(!box.IsSolid)
                    box.Destroyed = GL_TRUE;
                // 碰撞处理
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if(dir == LEFT || dir == RIGHT){ // 水平方向碰撞
                    Ball->Velocity.x = -Ball->Velocity.x;
                    // 重定位
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                    if(dir == LEFT)
                        Ball->Position.x += penetration; //将球体右移
                    else
                        Ball->Position.x -= penetration; //左移
                }
                else{ // 垂直方向
                    Ball->Velocity.y = -Ball->Velocity.y; //反转垂直速度
                    // 重定位
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                    if(dir == UP)
                        Ball->Position.y -= penetration; // 将球上移
                    else
                        Ball->Position.y += penetration;
                }
           }

        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if(!Ball->Stuck && std::get<0>(result)){
        // 检查碰到了挡板的哪个位置，并且根据碰到的哪个位置来改变速度
        GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
        GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        GLfloat percentage = distance / (Player->Size.x / 2);
        // 依据结果移动
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    }
}

// 经典方形AABB碰撞，遵循 LLRR 原则（x和y）
/*
GLboolean Game::CheckCollision(GameObject &one, GameObject &two){
    // x
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;

    // y
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;

    return collisionX && collisionY;
}
*/

float Game::clamp(float value, float min, float max){
    return std::max(min, std::min(max, value));
}

// AABB Circle collision detection 碰撞检测
/*
GLboolean Game::CheckCollision(BallObject &one, GameObject &two){
    // 获取球的中心坐标
    glm::vec2 center(one.Position + one.Radius);

    // half-width & half-height
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);

    // 获取B点坐标，即AABB中点
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    // 获取D，圆形终点C与AABB中点的差值，用于计算clamped值
    glm::vec2 difference = center - aabb_center;
    //                                            (-h | -w)              (h | w)
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // P点坐标
    glm::vec2 closest = aabb_center + clamped;
    // 判断圆心C和P的距离是否小于圆形半径(radius)
    difference = closest - center;
    return glm::length(difference) < one.Radius;

}
*/

Direction Game::VectorDirection(glm::vec2 target){
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f), // UP
        glm::vec2(1.0f, 0.0f), // RIGHT
        glm::vec2(0.0f, -1.0f),// DOWN
        glm::vec2(-1.0f, 0.0f) // LEFT
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for(GLuint i = 0; i < 4; i++){
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if(dot_product > max){
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision Game::CheckCollision(BallObject &one, GameObject &two){
    // 获取球的中心坐标
    glm::vec2 center(one.Position + one.Radius);

    // half-width & half-height
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);

    // 获取B点坐标，即AABB中点
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    // 获取D，圆形终点C与AABB中点的差值，用于计算clamped值
    glm::vec2 difference = center - aabb_center;
    //                                            (-h | -w)              (h | w)
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // P点坐标
    glm::vec2 closest = aabb_center + clamped;
    // 判断圆心C和P的距离是否小于圆形半径(radius)
    difference = closest - center;
    if(glm::length(difference) < one.Radius){
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    }
    else{
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
    }
}

void Game::ResetLevel(){
    if(this->Level == 0)
        this->Levels[0].Load("level/level1.txt", this->Width, this->Height / 2);
    else if(this->Level == 1)
        this->Levels[1].Load("level/level2.txt", this->Width, this->Height / 2);
    else if(this->Level == 2)
        this->Levels[2].Load("level/level3.txt", this->Width, this->Height / 2);
    else if(this->Level == 3)
        this->Levels[3].Load("level/level3.txt", this->Width, this->Height / 2);
    else if(this->Level == 4)
        this->Levels[4].Load("level/level4.txt", this->Width, this->Height / 2);
}

void Game::ResetPlayer(){
    //
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}