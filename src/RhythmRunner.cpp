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
#include <fstream>
#include <iomanip>

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
#include "GameCamera.h"
#include "Player.h"

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#include "GameState.h"
#include "GameRenderer.h"

#define RESOURCE_DIR "../Assets"
#define FRAMES_PER_SEC 120.0f
#define SEC_PER_FRAME 1 / FRAMES_PER_SEC
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480

static const std::shared_ptr<GameCamera> camera =
std::make_shared<GameCamera>();
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

static void CursorCallBack(GLFWwindow* window, double xpos, double ypos) {
  camera->pivot(WINDOW_WIDTH, WINDOW_HEIGHT, xpos, ypos);
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
}

int main(int argc, char** argv) {
  std::shared_ptr<GameRenderer> renderer = std::make_shared<GameRenderer>();
  std::shared_ptr<LevelGenerator> levelGenerator =
    std::make_shared<LevelGenerator>(MUSIC);

  std::shared_ptr<Level> level = std::make_shared<Level>(
      levelGenerator->getMusic(), levelGenerator->generateLevel());
  std::shared_ptr<GameCamera> camera = std::make_shared<GameCamera>();

  std::shared_ptr<Player> player = std::make_shared<Player>(glm::vec3(-3, 0, -5));

  std::shared_ptr<GameState> gameState =
    std::make_shared<GameState>(level, camera, player);
  std::shared_ptr<GameUpdater> updater = std::make_shared<GameUpdater>();


  renderer->Init(RESOURCE_DIR, gameState, ErrorCallback, KeyCallback,
      MouseCallback, ResizeCallback, CursorCallBack);
  // fix ur time step
  double clock = glfwGetTime();
#ifdef DEBUG
  float elapsed[25] = {1};
  int pos = 0;
#endif
  while (!glfwWindowShouldClose(renderer->GetWindow())) {
    double nextTime = glfwGetTime();
    if (nextTime - clock > SEC_PER_FRAME) {
      updater->Update(gameState);
      renderer->Render(gameState);

#ifdef DEBUG
      elapsed[pos++] = SEC_PER_FRAME;
      pos %= 25;
      float tot = 0;
      for (int i = 0; i < 25; i++)
        tot += elapsed[i];

      std::cout << "\r" << std::setw(10) << std::setprecision(4)
        << "FPS: " << 25.0f / tot;
#endif
      clock = nextTime;
    }
  }
  renderer->Close();
  std::shared_ptr<GameState> game_state;

  return EXIT_SUCCESS;
}
