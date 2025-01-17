#include "gameLevel.h"
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ballObject.h"
#include "particle_generator.h"
#include "textRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>
#include <string>

#include <iostream>

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

const float BALL_RADIUS = 12.5f;

BallObject *Ball;

SpriteRenderer *Renderer;
GameObject *Player;

TextRenderer *Text;

ParticleGenerator *Particles;

float fpsTimer = 0.0f;
int FPScounter = 0;
int FPS = 0;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), Width(width), Height(height)
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

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("font/OCRAEXT.TTF", 24);

    // 粒子着色器
    ResourceManager::LoadShader("shader/particle.vs", "shader/particle.fs", nullptr, "particle");

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

    ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);

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

    ResourceManager::LoadTexture("texture/particle.png", GL_TRUE, "particle");

    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    // load levels

    GameLevel one; one.Load("level/level1.txt", this->Width, this->Height / 2);
    GameLevel two; two.Load("level/level2.txt", this->Width, this->Height / 2);
    GameLevel three; three.Load("level/level3.txt", this->Width, this->Height / 2);
    GameLevel four; four.Load("level/level4.txt", this->Width, this->Height / 2);

    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // particles 粒子数组构建
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );

    this->Lives = 3;

    this->Score = 0;
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    this->DoCollisions();

    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

    if(Ball->Position.y >= this->Height){ // 球体是否接触到底部边界
        this->Lives--;
        // 检查生命值
        if(this->Lives == 0){
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }

    if(this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()){
        this->ResetLevel();
        this->ResetPlayer();
        //Effects
        this->State = GAME_WIN;
    }
}

void Game::ProcessInput(float dt)
{
    if(this->State == GAME_MENU){
        if(this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]){
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if(this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]){
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }
        if(this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]){
            if(this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            
            this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }
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
    if(this->State == GAME_WIN){
        if(this->Keys[GLFW_KEY_ENTER]){
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            this->State = GAME_MENU;
        }
    }
}

void Game::Render(float dt)
{
    if(this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN){
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level

        this->Levels[this->Level].Draw(*Renderer);

        Particles->Draw();

        Ball->Draw(*Renderer);

        Player->Draw(*Renderer);

        std::stringstream ss; ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);

        // FPS渲染
        fpsTimer += dt;
        FPScounter++;
        if(fpsTimer >= 1.0f){
            fpsTimer = 0.0f;
            FPS = FPScounter;
            FPScounter = 0;
        } 
        Text->RenderText("FPS:" + std::to_string(FPS), 750.0f, 5.0f, 0.5f);
        Text->RenderText("Score:" + std::to_string(Score), 300.0f, 5.0f, 2.0f);
    }
    if(this->State == GAME_MENU){
        Text->RenderText("Press ENTER to start", 250.0f, Height / 2, 1.0f);
        Text->RenderText("Press W or S to select level", 245.0f, Height / 2 + 20.0f, 0.75f);
    }
    if(this->State == GAME_WIN){
        Text->RenderText("You WON !!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
    }
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
                if(!box.IsSolid){
                    box.Destroyed = GL_TRUE;
                    this->Score += box.Score;
                }
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
    this->Lives = 3;
    this->Score = 0;

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