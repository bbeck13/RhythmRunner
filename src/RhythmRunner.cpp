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
#include "Sky.h"
#include "VideoTexture.h"
#include "json.hpp"
#include "LevelJson.h"
#include "Sky.h"
#include "VideoTexture.h"
#include "FileSystemUtils.h"
#include "ParticleGenerator.h"

#define MUSIC "music/3.wav"
#define LEVEL "levels/demo_level"

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

  MainProgramMode program_mode;

  GameUpdater game_updater;
  MenuRenderer menu_renderer;
  GameRenderer game_renderer;
  game_renderer.Init(ASSET_DIR, window);

  // Start the game at the Menu
  std::shared_ptr<GameState> game_state;
  std::shared_ptr<MenuState> menu_state = std::make_shared<MenuState>();
  menu_state->SetMusicPath(ASSET_DIR "/" MUSIC);
  menu_state->SetLevelPath(ASSET_DIR "/" LEVEL);
  program_mode = MainProgramMode::MENU_SCREEN;

  while (!glfwWindowShouldClose(window)) {
    switch (program_mode) {
      case MainProgramMode::CREATE_NEW_GAME: {
        LevelGenerator* level_generator;
        if (FileSystemUtils::FileExists(menu_state->GetLevelPath())) {
          std::ifstream input(menu_state->GetLevelPath());
          nlohmann::json leveljson;
          input >> leveljson;

          std::vector<std::shared_ptr<GameObject>> level = leveljson;
          std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> lvl =
              std::make_shared<std::vector<std::shared_ptr<GameObject>>>(level);

          level_generator = new LevelGenerator(menu_state->GetMusicPath(), lvl);
        } else {
          level_generator = new LevelGenerator(menu_state->GetMusicPath());
        }
        game_state = std::make_shared<GameState>(
            level_generator->generateLevel(), std::make_shared<GameCamera>(),
            std::make_shared<Player>(), std::make_shared<Sky>(), window);
        // Only Video texture we have right now
        std::shared_ptr<VideoTexture> vid = std::make_shared<VideoTexture>(
            std::string(ASSET_DIR) + "/textures/sky");
        game_state->AddVideoTexture("sky", vid);
        game_updater.Init(game_state);
        delete level_generator;
        program_mode = MainProgramMode::SET_CAMERA;

        // continue to SET_CAMERA
      }
      case MainProgramMode::SET_CAMERA: {
        game_updater.UpdateCamera(game_state);
        game_renderer.RenderCameraSetup(window, game_state);
        if (InputBindings::KeyDown(GLFW_KEY_ENTER)) {
          program_mode = MainProgramMode::RESET_GAME;
          // clear buffered keypresses when starting the game
          InputBindings::ClearKeyPresses();
        }
        break;
      }
      case MainProgramMode::RESET_GAME:
        game_updater.Reset(game_state);
        // lock cursor when starting game
        InputBindings::SetCursorMode(InputBindings::CursorMode::LOCKED);
      // continue to GAME_SCREEN
      case MainProgramMode::GAME_SCREEN: {
        switch (game_state->GetPlayingState()) {
          case GameState::PlayingState::FAILURE:
          case GameState::PlayingState::SUCCESS:
            game_updater.PostGameUpdate(game_state);
            break;
          case GameState::PlayingState::PAUSED:
            break;
          case GameState::PlayingState::PLAYING: {
            uint64_t target_tick_count =
                game_updater.CalculateTargetTicks(game_state);
            // Run enough ticks to catch up
            // TODO(jarhar): consider basic infinite loop detection here
            // TODO(jarhar): consider re-assessing timing after each call to
            // Update().
            //  What if the music starts/stops during one of multiple Updates?
            while (game_state->GetElapsedTicks() < target_tick_count) {
              game_updater.Update(game_state);
            }
            break;
          }
        }

        program_mode = game_renderer.Render(window, game_state);
        break;
      }

      case MainProgramMode::MENU_SCREEN:
        program_mode = menu_renderer.Render(window, menu_state);
        break;

      case MainProgramMode::EXIT:
        RendererSetup::Close(window);
        return EXIT_SUCCESS;
    }

    PrintStatus();
  }

  RendererSetup::Close(window);
  return EXIT_SUCCESS;
}
