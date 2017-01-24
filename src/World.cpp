#include <fstream>
#include <iostream>
#include <iomanip>
#include "World.h"
#include "Renderer.h"
#include "Engine.h"
#include "GroundRenderer.h"
#include "BallRenderer.h"

World::World(Window *window) {
   this->window = window;
   this->renderer = new Renderer();
}

void World::setClearColor(vec4 clearColor) {
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
}

World::~World() {
   delete window;
}

void World::init() {
   GLSL::checkVersion();
   glClearColor(0.30f, 0.5f, 0.78f, 1.0f);

   glEnable(GL_DEPTH_TEST);
   GroundRenderer *gr = new GroundRenderer();
   gr->setMaterial(BLUE_PLASTIC);
   BallRenderer *br = new BallRenderer();
   br->setMaterial(BLUE_PLASTIC);
   renderer->init();

   Engine::insertCallback(GLFW_KEY_ESCAPE, [] (int scancode, int action, int mods) {
      if (action == GLFW_PRESS) {
         Window::setClose(true);
      }
   });
}

void World::update() {
   double clock = glfwGetTime();

#ifdef DEBUG
   float elapsed[25] = {1};
   int pos = 0;
#endif
   while (!window->shouldClose()) {
      double nextTime = glfwGetTime();
      if (nextTime - clock > SEC_PER_FRAME) {
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         renderer->render();
         window->swapBuffers();

#ifdef DEBUG
         elapsed[pos++] = SEC_PER_FRAME;
         pos %= 25;
         float tot = 0;
         for (int i = 0; i < 25; i ++)
            tot += elapsed[i];

         cout << "\r" << setw(10) << setprecision(3)
              << "FPS: " << 25.0f / tot;
#endif
         clock = nextTime;
      }

      glfwPollEvents();
      if (glGetError() != GL_NO_ERROR) {
         window->setClose(true);
      }
   }
}
