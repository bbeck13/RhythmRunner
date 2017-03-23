//
//  Particles.cpp
//  RhythmRunner
//
//  Created by Alex on 3/3/17.
//
//

#include "ParticleGenerator.h"

#include <algorithm>

static GLfloat RandomVelocity() {
  return ((rand() % 10) - 5) / 100.0f;
}

static GLfloat RandomColor() {
  return 0.1 + ((rand() % 100) / 100.0f);
}

ParticleGenerator::ParticleGenerator(GLuint amount) : amount(amount) {
  this->init();
}

void ParticleGenerator::Update(GLuint newParticles,
                               std::shared_ptr<Player> object,
                               glm::vec3 offset) {
  for (GLuint i = 0; i < newParticles; ++i) {
    int unusedParticle = this->firstUnusedParticle();

    glm::vec3 new_velocity(std::abs(RandomVelocity() * 5),
                           std::abs(RandomVelocity()),
                           RandomVelocity() + object->GetZVelocity() * 0.5f);

    this->respawn(this->particles[unusedParticle], object, new_velocity,
                  glm::vec4(RandomColor(), RandomColor(), RandomColor(), 1.0f),
                  offset);
  }

  for (GLuint i = 0; i < this->amount; ++i) {
    Particle& p = this->particles[i];
    if (p.Life > 0.0f) {
      p.Life -= 1.0f;
      p.Position -= p.Velocity;
      p.Color.a -= 2.5;
    }
  }
}

void ParticleGenerator::DrawParticles(const std::shared_ptr<Program> prog) {
  int h_pos = -1;

  glDepthMask(GL_FALSE);
  for (Particle particle : this->particles) {
    if (particle.Life > 0.0f) {
      glUniform3f(prog->getUniform("Offset"), particle.Position.x,
                  particle.Position.y, particle.Position.z);
      glUniform4f(prog->getUniform("Color"), particle.Color.x, particle.Color.y,
                  particle.Color.z, 1.0f);
      glBindVertexArray(this->VAO);
      h_pos = prog->getAttribute("vertPos");
      glEnableVertexAttribArray(h_pos);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glVertexAttribPointer(h_pos, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }
  }

  glDepthMask(GL_TRUE);
}

void ParticleGenerator::init() {
  GLfloat particle_quad[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                             1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(this->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (GLvoid*)0);
  glBindVertexArray(0);

  for (GLuint i = 0; i < this->amount; ++i) {
    this->particles.push_back(Particle());
  }
}

GLuint ParticleGenerator::firstUnusedParticle() {
  GLuint lastUsedParticle = 0;
  for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }

  for (GLuint i = 0; i < lastUsedParticle; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }

  lastUsedParticle = 0;
  return 0;
}

void ParticleGenerator::SortParticles(std::shared_ptr<GameCamera> camera) {
  std::sort(particles.begin(), particles.end(),
            ParticleSort(camera->getPosition()));
}

void ParticleGenerator::respawn(Particle& particle,
                                std::shared_ptr<Player> object,
                                glm::vec3 velocity,
                                glm::vec4 color,
                                glm::vec3 offset) {
  GLfloat random = ((rand() % 11) - 5) / 10.0f;
  particle.Position =
      glm::vec3(object->GetPosition().x + offset.x + random * 1.3 - 0.5,
                object->GetPosition().y + offset.y + random,
                object->GetPosition().z + offset.z + random);
  particle.Color = color;
  particle.Life = 80.0f;
  particle.Velocity = velocity;
}

// used for jumping and landing
void ParticleGenerator::SpawnAll(std::shared_ptr<Player> player,
                                 glm::vec3 offset,
                                 glm::vec3 color_multiplier) {
  for (Particle& particle : particles) {
    float rotation = (rand() / (float)RAND_MAX) * M_PI * 2;
    float magnitude = rand() / (float)RAND_MAX;
    glm::vec3 new_velocity(magnitude * std::cos(rotation),
                           std::abs(RandomVelocity()),
                           magnitude * std::sin(rotation));
    new_velocity *= 0.1f;
    new_velocity +=
        glm::vec3(-DELTA_X_PER_TICK / 2, 0, player->GetZVelocity() / 2);

    glm::vec3 new_color(RandomColor(), RandomColor(), RandomColor());
    new_color *= color_multiplier;

    respawn(particle, player, new_velocity, glm::vec4(new_color, 1.0f), offset);
  }
}
