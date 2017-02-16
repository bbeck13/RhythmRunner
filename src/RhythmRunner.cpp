// Braden Beck (bnbeck)

#include <fstream>
#include <iomanip>
#include <iostream>
#include <GL/glew.h>

#include "GameCamera.h"
#include "GameRenderer.h"
#include "GameState.h"
#include "GameUpdater.h"
#include "InputBindings.h"
#include "Level.h"
#include "LevelGenerator.h"
#include "MenuRenderer.h"
#include "RendererSetup.h"
#include "TimingConstants.h"

#define MUSIC "music/2.wav"

enum class Renderer { GAME, MENU };

std::shared_ptr<GameState> CreateGame(std::string music_path) {
  LevelGenerator level_generator(music_path);
  return std::make_shared<GameState>(level_generator.generateLevel(),
                                     std::make_shared<GameCamera>(),
                                     std::make_shared<Player>());
}

void PrintStatus() {
#ifdef DEBUG
  static double last_debug_time = glfwGetTime();
  static int frames_since_last_debug = 0;

  double current_debug_time = glfwGetTime();
  double elapsed_debug_time = current_debug_time - last_debug_time;
  if (elapsed_debug_time > 1.0) {
// more than a second has elapsed, so print an update
#ifdef _WIN32  // printing \r doesn't work on windows
    std::cout << std::setw(10) << std::setprecision(4)
              << "FPS: " << (frames_since_last_debug / elapsed_debug_time)
              << std::endl;
#else
    std::cout << "\r" << std::setw(10) << std::setprecision(4)
              << "FPS: " << (frames_since_last_debug / elapsed_debug_time)
              << std::flush;
#endif
    last_debug_time = current_debug_time;
    frames_since_last_debug = 0;
  }

  frames_since_last_debug++;
#endif
}

int main(int argc, char** argv) {
  GLFWwindow* window = RendererSetup::InitOpenGL();
  InputBindings::Bind(window);

  GameRenderer game_renderer;
  GameUpdater game_updater;
  game_renderer.Init(ASSET_DIR);
  std::shared_ptr<GameState> game_state;

  MenuRenderer menu_renderer;
  std::shared_ptr<MenuState> menu_state;

  // Start the game at the Menu
  menu_state = std::make_shared<MenuState>();
  menu_state->SetMusicPath(ASSET_DIR "/" MUSIC);
  Renderer current_renderer = Renderer::MENU;

  double initial_glfw_time = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    switch (current_renderer) {
      case Renderer::GAME:
        // Render first, then catch up in Update()s
        // TODO(jarhar): Does this order make more sense than the opposite?
        game_renderer.Render(window, game_state);

        // TODO(jarhar): in the future we may want to change the music timing
        // mode
        // here. If there is no music playing, then we can't synchronize the
        // game
        // ticks to music. Right now, since we are always playing music, we will
        // always be in music timing mode, but when we have a main menu or pause
        // or
        // something, "game_state->SetMusicTimingMode(false)" and
        // "initial_glfw_time
        // = glfwGetTime()" will have to be called somewhere.

        // Calculate what tick we should be at right now
        unsigned target_tick_count;
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

        // Run enough ticks to catch up
        // TODO(jarhar): consider basic infinite loop detection here
        // TODO(jarhar): consider re-assessing timing after each call to
        // Update().
        //  What if the music starts/stops during one of multiple Update()s?
        while (game_state->GetElapsedTicks() < target_tick_count) {
          game_updater.Update(game_state);
        }

        // TODO(jarhar): replace this with a final score screen or something
        if (game_state->Done()) {
          glfwSetWindowShouldClose(window, GL_TRUE);
        }

        break;
      case Renderer::MENU:
        menu_renderer.Render(window, menu_state);

        // Check for a switch to game mode
        if (menu_state->GetMenuMode() == MenuMode::START_GAME) {
          // start a new game
          game_state = CreateGame(menu_state->GetMusicPath());
          menu_state->SetMenuMode(MenuMode::WAITING_FOR_INPUT);
          game_updater.Init(game_state);
          current_renderer = Renderer::GAME;
        }

        break;
    }

    PrintStatus();
  }

  RendererSetup::Close(window);
  return EXIT_SUCCESS;
}
