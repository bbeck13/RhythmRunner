//
//  Particles.hpp
//  RhythmRunner
//
//  Created by Alex on 3/6/17.
//
//

#ifndef PARTICLES_H_
#define PARTICLES_H_

#include "GameObject.h"
#include "Player.h"
#include "Program.h"

#define DEFAULT_PARTICLE_COUNT 5000

class ParticleGenerator {
 public:
  struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
  };

  ParticleGenerator(GLuint amount = DEFAULT_PARTICLE_COUNT);
  void Update(GLuint newParticles,
              std::shared_ptr<Player> object,
              glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
  void DrawParticles(const std::shared_ptr<Program> prog);
  std::vector<Particle> particles;

 private:
  GLuint amount;
  GLuint VBO;
  GLuint VAO;
  void init();
  GLuint firstUnusedParticle();
  void respawn(Particle& particle,
               std::shared_ptr<Player> object,
               glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif /* PARTICLES_H */
