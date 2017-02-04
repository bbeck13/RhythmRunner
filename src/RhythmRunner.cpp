// Braden Beck (bnbeck)

#include <fstream>
#include <iomanip>
#include <iostream>

#include "GameCamera.h"
#include "GameRenderer.h"
#include "GameState.h"
#include "GameUpdater.h"
#include "Level.h"
#include "LevelGenerator.h"
#include "TimingConstants.h"

#define RESOURCE_DIR "../Assets"
#define FRAMES_PER_SEC 120.0f
#define SEC_PER_FRAME 1 / FRAMES_PER_SEC
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480
#define MUSIC "music/1.wav"

static const std::shared_ptr<GameCamera> camera =
    std::make_shared<GameCamera>();
static const std::shared_ptr<Player> player = std::make_shared<Player>();

int main(int argc, char** argv) {
  std::shared_ptr<GameRenderer> renderer = std::make_shared<GameRenderer>();
  std::string music;
  music = ASSET_DIR;
  music += "/";
  music += MUSIC;

  std::shared_ptr<LevelGenerator> levelGenerator =
      std::make_shared<LevelGenerator>(music);

  std::shared_ptr<Level> level = levelGenerator->generateLevel();

  std::shared_ptr<GameState> gameState =
      std::make_shared<GameState>(level, camera, player);
  std::shared_ptr<GameUpdater> updater = std::make_shared<GameUpdater>();

  renderer->Init(RESOURCE_DIR, gameState);
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
      elapsed[pos++] = glfwGetTime() - clock;
      pos %= 25;
      float tot = 0;
      for (int i = 0; i < 25; i++)
        tot += elapsed[i];

      std::cout << "\r" << std::setw(10) << std::setprecision(4)
                << "FPS: " << 25.0f / tot
                << " Score: " << gameState->GetPlayer()->GetScore();
#endif
      clock = nextTime;
    }
  }
  renderer->Close();

  return EXIT_SUCCESS;
}
