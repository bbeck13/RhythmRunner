// Braden Beck (bnbeck)

#include <fstream>
#include <iomanip>
#include <iostream>

#include "RendererSetup.h"
#include "GameCamera.h"
#include "GameRenderer.h"
#include "GameState.h"
#include "GameUpdater.h"
#include "InputBindings.h"
#include "Level.h"
#include "LevelGenerator.h"
#include "MenuRenderer.h"
#include "TimingConstants.h"
#include "EndRenderer.h"
#include "Sky.h"
#include "VideoTexture.h"

#define MUSIC "music/2.wav"

std::shared_ptr<MenuState> menu_state;
std::shared_ptr<GameState> game_state;

GameUpdater game_updater;

void CreateGame() {
  LevelGenerator level_generator(menu_state->GetMusicPath());
  game_state = std::make_shared<GameState>(level_generator.generateLevel(),
                                           std::make_shared<GameCamera>(),
                                           std::make_shared<Player>(),
                                           std::make_shared<Sky>());
  // Only Video texture we have right now
  std::shared_ptr<VideoTexture> vid = std::make_shared<VideoTexture>(std::string(ASSET_DIR) + "/textures/sky");
  game_state->AddVideoTexture("sky", vid);
  game_updater.Init(game_state);
}

void PrintStatus() {
#ifdef DEBUG
  static double last_debug_time = glfwGetTime();
  static int frames_since_last_debug = 0;

  double current_debug_time = glfwGetTime();
  double elapsed_debug_time = current_debug_time - last_debug_time;
  if (elapsed_debug_time > 1.0) {
// more than a second has elapsed, so print an update
#ifndef _WIN32  // printing \r doesn't work on windows
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

  ProgramMode program_mode;

  MenuRenderer menu_renderer;
  EndRenderer end_renderer;
  GameRenderer game_renderer;
  game_renderer.Init(ASSET_DIR);

  // Start the game at the Menu
  menu_state = std::make_shared<MenuState>();
  menu_state->SetMusicPath(ASSET_DIR "/" MUSIC);
  program_mode = ProgramMode::MENU_SCREEN;

  while (!glfwWindowShouldClose(window)) {
    // TODO(jarhar): this is kinda hacky
    if (program_mode != ProgramMode::GAME_SCREEN) {
      InputBindings::StoreKeypresses();
    }

    switch (program_mode) {
      case ProgramMode::GAME_SCREEN: {
        // Render first, then catch up in Update()s
        // TODO(jarhar): Does this order make more sense than the opposite?
        game_renderer.Render(window, game_state);

        // get time elapsed in music
        // then, calculate target ticks elapsed based on music time elapsed
        int64_t music_offset_micros = game_state->GetLevel()
                                          ->getMusic()
                                          ->getPlayingOffset()
                                          .asMicroseconds();
        unsigned target_tick_count = music_offset_micros * TICKS_PER_MICRO +
                                     game_state->GetTimingStartTick();
        bool song_done = game_state->GetLevel()->getMusic()->getStatus() ==
                         sf::Music::Status::Stopped;

        // Run enough ticks to catch up
        // TODO(jarhar): consider basic infinite loop detection here
        // TODO(jarhar): consider re-assessing timing after each call to
        // Update().
        //  What if the music starts/stops during one of multiple Update()s?
        while (game_state->GetElapsedTicks() < target_tick_count) {
          InputBindings::StoreKeypresses();
          game_updater.Update(game_state);
        }

        if (game_state->Done() || song_done) {
          program_mode = ProgramMode::END_SCREEN;
        }
        break;
      }

      case ProgramMode::MENU_SCREEN:
        program_mode = menu_renderer.Render(window, menu_state);

        if (program_mode == ProgramMode::GAME_SCREEN) {
          // If we are switching to game mode, then create a new game
          CreateGame();
        }
        break;

      case ProgramMode::END_SCREEN:
        program_mode = end_renderer.Render(window, game_state);
        if (program_mode == ProgramMode::GAME_SCREEN) {
          game_updater.Reset(game_state);
        }
        break;

      case ProgramMode::EXIT:
        RendererSetup::Close(window);
        return EXIT_SUCCESS;
    }

    PrintStatus();
  }

  RendererSetup::Close(window);
  return EXIT_SUCCESS;
}
