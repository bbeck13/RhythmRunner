/**
 * RhythmRunner.cpp
 *
 *
 * For now this is just a bunch of import statements to make assert the
 * dependencies
 * Braden Beck (bnbeck)
 *
 */

/*cpp includes*/
#include <iostream>

/* Aquila stuff*/
#include <aquila/global.h>
#include <aquila/source/WaveFile.h>
#include <aquila/tools/TextPlot.h>
#include <aquila/transform/AquilaFft.h>
#include <aquila/transform/OouraFft.h>
#include <aquila/source/generator/SineGenerator.h>
#include <aquila/transform/FftFactory.h>

/*SFML*/
#include <SFML/Audio.hpp>

/*GL stuff glew glfw etc..*/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include "Level.h"
#include "GameUpdater.h"
#include "LevelGenerator.h"

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#include "GameState.h"
#include "GameRenderer.h"

#define RESOURCE_DIR "../Assets"
#define FRAMES_PER_SEC 120.0f
#define SEC_PER_FRAME 1 / FRAMES_PER_SEC

// Callbacks seem like they should be in their own file/class
// Still trying to figure out function pointer or whatever these are
static void KeyCallback(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
  }
}

static void MouseCallback(GLFWwindow* window,
                          int button,
                          int action,
                          int mods) {
  double posX, posY;
  if (action == GLFW_PRESS) {
    glfwGetCursorPos(window, &posX, &posY);
  }
}

static void ErrorCallback(int error, const char* description) {
  std::cerr << description << std::endl;
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
  // glViewport(0, 0, width, height);
}

int main(int argc, char** argv) {
  std::shared_ptr<GameRenderer> renderer = std::make_shared<GameRenderer>();
  std::shared_ptr<LevelGenerator> levelGenerator =
      std::make_shared<LevelGenerator>(MUSIC);

  std::shared_ptr<Level> level = std::make_shared<Level>(
      levelGenerator->getMusic(), levelGenerator->generateLevel());
  std::shared_ptr<GameState> gameState = std::make_shared<GameState>(level);

  std::shared_ptr<GameUpdater> updater = std::make_shared<GameUpdater>();

  renderer->Init(RESOURCE_DIR, NULL, ErrorCallback, KeyCallback, MouseCallback,
                 ResizeCallback);
  // fix ur time step
  double clock = glfwGetTime();
  while (!glfwWindowShouldClose(renderer->GetWindow())) {
    double nextTime = glfwGetTime();
    if (nextTime - clock > SEC_PER_FRAME) {
      updater->Update(gameState);
      renderer->Render(gameState);

      clock = nextTime;
    }
  }
  renderer->Close();
  std::shared_ptr<GameState> game_state;

  return EXIT_SUCCESS;
}
