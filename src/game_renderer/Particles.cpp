  //
//  Particles.cpp
//  RhythmRunner
//
//  Created by Alex on 3/3/17.
//
//

#include "Particles.h"

ParticleGenerator::ParticleGenerator(GLuint amount)
: amount(amount)
{
    this->init();
}

void ParticleGenerator::Update(GLuint newParticles, std::shared_ptr<Player> object, glm::vec3 offset)
{
    for(GLuint i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawn(this->particles[unusedParticle], object, offset);
    }
    
    for(GLuint i = 0; i < this->amount; ++i)
    {
        Particle &p = this->particles[i];
        p.Life -= 1.0f;
        if(p.Life > 0.0f)
        {
            p.Position -= p.Velocity ;
            p.Color.a -=  2.5;
        }
    }
}

void ParticleGenerator::DrawParticles(const std::shared_ptr<Program> prog)
{
    int h_pos = -1;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 
    for(Particle particle : this->particles)
    {
        if(particle.Life > 0.0f)
        {
            glUniform3f(prog->getUniform("Offset"), particle.Position.x,
                        particle.Position.y, particle.Position.z);
            glUniform4f(prog->getUniform("Color"), particle.Color.x,
                        particle.Color.y, particle.Color.z, 1.0f);
            glBindVertexArray(this->VAO);
            h_pos = prog->getAttribute("vertPos");
            glEnableVertexAttribArray(h_pos);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(h_pos, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    GLfloat particle_quad[] = {
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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    for (GLuint i = 0; i < this->amount; ++i)
    {
        this->particles.push_back(Particle());
    }
}

GLuint ParticleGenerator::firstUnusedParticle()
{
    GLuint lastUsedParticle = 0;
    for(GLuint i = lastUsedParticle; i < this->amount; ++i)
    {
        if(this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    
    for (GLuint i = 0; i < lastUsedParticle; ++i)
    {
        if(this->particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }
    
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawn(Particle &particle, std::shared_ptr<Player> object, glm::vec3 offset)
{
    GLfloat random = ((rand() % 11) - 5) / 10.0f;
    GLfloat rV1 = ((rand() % 10) - 5) / 100.0f;
    GLfloat rV2 = ((rand() % 10) - 5)/ 100.0f;
    GLfloat rV3 = ((rand() % 10) - 5)/ 100.0f;
    GLfloat rColor1 = 0.1 + ((rand() % 100) / 100.0f);
    GLfloat rColor2 = 0.1 + ((rand() % 100) / 100.0f);
    GLfloat rColor3 = 0.1 + ((rand() % 100) / 100.0f);
    particle.Position = glm::vec3(object->GetPosition().x + random * 1.3 - 0.5, object->GetPosition().y + random, object->GetPosition().z + random);
    particle.Color = glm::vec4(rColor1, rColor2, rColor3, 1.0f);
    particle.Life = 80.0f;
    particle.Velocity = glm::vec3(rV1, object->GetYVelocity() * 0.5f + rV2,object->GetZVelocity() * 0.5f + rV3);
    if(object->GetZVelocity() == 1)
    {
        particle.Velocity = glm::vec3(rV1 * 5, object->GetYVelocity() * 0.5f + rV2 * 5,rV3 * 5);
        particle.Color = glm::vec4(1.0f, rColor2, rColor3 / 2, 0.5f);
    }
}
    

