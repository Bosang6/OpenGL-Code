#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount){
        this->init();
}

void ParticleGenerator::init(){
        unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    }; 
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

void ParticleGenerator::Update(float dt, GameObject &object, unsigned int newParticles, glm::vec2 offset){
    // 添加新的粒子
    for(unsigned int i = 0; i < newParticles; ++i){
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // 更新粒子
    for(unsigned int i = 0; i < this->amount; ++i){
        Particle &p = this->particles[i];
        p.Life -= dt;
        if(p.Life > 0.0f){
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}

void ParticleGenerator::Draw(){
    // blending 
    // alpha加亮叠加
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for(Particle particle : this->particles){
        if(particle.Life > 0.0f){
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // 半透明混合模式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle(){
    // 从第一个消失的粒子开始寻找下一个生命值耗完的粒子，通常很快就能找到
    for(int i = lastUsedParticle; i < this->amount; i++){
        if(particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }

    //如果没找到，就从头开始找
    for(int i = 0; i < lastUsedParticle; i++){
        if(particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }

    // 如果所有的粒子都还活着
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset){
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}