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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480
#define MUSIC "music/1.wav"

int main(int argc, char** argv) {
  std::string music;
  music = ASSET_DIR;
  music += "/";
  music += MUSIC;

  std::shared_ptr<LevelGenerator> levelGenerator =
      std::make_shared<LevelGenerator>(music);

  std::shared_ptr<Level> level = levelGenerator->generateLevel();

  std::shared_ptr<GameState> game_state = std::make_shared<GameState>(
      level, std::make_shared<GameCamera>(), std::make_shared<Player>());

  std::shared_ptr<GameRenderer> renderer = std::make_shared<GameRenderer>();
  renderer->Init(ASSET_DIR, game_state);

  std::shared_ptr<GameUpdater> updater = std::make_shared<GameUpdater>();
  updater->Init(game_state);

  double initial_glfw_time = glfwGetTime();
  // get one update in before starting rendering?
  // TODO(jarhar): if this isnt necessary to set up the game it should be
  // removed. Update() was originally called before Render()
  updater->Update(game_state);

  while (!renderer->WindowShouldClose()) {
    // Render first, then catch up in Update()s
    // TODO(jarhar): Does this order make more sense than the opposite?
    renderer->Render(game_state);

    // TODO(jarhar): in the future we may want to change the music timing mode
    // here. If there is no music playing, then we can't synchronize the game
    // ticks to music. Right now, since we are always playing music, we will
    // always be in music timing mode, but when we have a main menu or pause or
    // something, "game_state->SetMusicTimingMode(false)" and "initial_glfw_time
    // = glfwGetTime()" will have to be called somewhere.

    // Calculate what tick we should be at right now
    unsigned target_tick_count;
#ifdef DEBUG
    double clock = glfwGetTime();
    float elapsed[25] = {1};
    int pos = 0;
#endif
    if (game_state->GetMusicTimingMode()) {
      // get time elapsed in music
      // then, calculate target ticks elapsed based on music time elapsed
      int64_t music_offset_micros = game_state->GetLevel()
                                        ->getMusic()
                                        ->getPlayingOffset()
                                        .asMicroseconds();
      target_tick_count = music_offset_micros * TICKS_PER_MICRO +
                          game_state->GetTimingStartTick();
    } else {
      int64_t elapsed_seconds = glfwGetTime() - initial_glfw_time;
      target_tick_count = elapsed_seconds * TICKS_PER_SECOND;
    }

#ifdef DEBUG
    double nextTime = glfwGetTime();
#endif

    // Run enough ticks to catch up
    // TODO(jarhar): consider basic infinite loop detection here
    // TODO(jarhar): consider re-assessing timing after each call to Update().
    //  What if the music starts/stops during one of multiple Update()s?
    while (game_state->GetElapsedTicks() < target_tick_count) {
      updater->Update(game_state);
    }

#ifdef DEBUG  // Print FPS and stuff
    elapsed[pos++] = glfwGetTime() - clock;
    pos %= 25;
    float tot = 0;
    for (int i = 0; i < 25; i++)
      tot += elapsed[i];

    std::cout << "\r" << std::setw(10) << std::setprecision(4)
              << "FPS: " << 25.0f / tot
              << " Score: " << gameState->GetPlayer()->GetScore() << std::flush;

    clock = nextTime;
#endif
  }

  renderer->Close();
  return EXIT_SUCCESS;
}
