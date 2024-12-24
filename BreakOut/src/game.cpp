
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer *Renderer;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    ResourceManager::LoadShader("shader/spriteVertex.vs", "shader/spriteFragment.fs", nullptr, "sprite");

    Shader &spriteShader = ResourceManager::GetShader("sprite");

    glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);

    spriteShader.Use().SetInteger("image", 0);
    spriteShader.SetMatrix4("projection", projection);

    //ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    //ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(spriteShader);
    //Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    ResourceManager::LoadTexture("texture/awesomeface.png", true, "face");
}

void Game::Update(float dt)
{
    
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    Renderer->DrawSprite(ResourceManager::GetTexture("face"),
        glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}