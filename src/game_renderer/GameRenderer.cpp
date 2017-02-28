// Alex Ottoboni

#include "GameRenderer.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <queue>

#include "FileSystemUtils.h"
#include "InputBindings.h"
#include "LevelGenerator.h"
#include "MatrixStack.h"
#include "MovingPlatform.h"
#include "Octree.h"
#include "Platform.h"
#include "ViewFrustumCulling.h"
#include "json.hpp"
#include "RendererSetup.h"

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

void GameRenderer::Init(const std::string& resource_dir) {
  glClearColor(.2f, .2f, .2f, 1.0f);
  // Initialize all programs from JSON files in assets folder
  std::shared_ptr<Program> temp_program;
  std::vector<std::string> json_files =
      FileSystemUtils::ListFiles(std::string(ASSET_DIR) + "/shaders", "*.json");
  for (int i = 0; i < json_files.size(); i++) {
    temp_program = GameRenderer::ProgramFromJSON(json_files[i]);
    programs[temp_program->getName()] = temp_program;
  }

  std::shared_ptr<Texture> temp_texture;
  std::vector<std::string> texture_files = FileSystemUtils::ListFiles(
      std::string(ASSET_DIR) + "/textures", "*.json");
  for (int i = 0; i < texture_files.size(); i++) {
    temp_texture = GameRenderer::TextureFromJSON(texture_files[i]);
    textures[temp_texture->getName()] = temp_texture;
  }

  // Set up rainbow of colors in color_vector
  // TODO move somewhere else
  color_vec.push_back(glm::vec3(236.0 / 255.0, 0, 83.0 / 255.0));
  color_vec.push_back(glm::vec3(236.0 / 255.0, 122.0 / 255, 0));
  color_vec.push_back(glm::vec3(236.0 / 255.0, 205.0 / 255, 0));
  color_vec.push_back(glm::vec3(89.0 / 255.0, 236.0 / 255, 0));
  color_vec.push_back(glm::vec3(0 / 255.0, 172.0 / 255, 236.0 / 255.0));
  color_vec.push_back(glm::vec3(150.0 / 255.0, 0 / 255, 236.0 / 255.0));
}

std::shared_ptr<Texture> GameRenderer::TextureFromJSON(std::string filepath) {
  // Read in the file
  std::ifstream json_input_stream(filepath, std::ifstream::in);
  // Read the flle into the JSON library
  nlohmann::json json_handler;
  json_input_stream >> json_handler;

  std::string filename = json_handler["filename"];
  std::string name = json_handler["name"];
  int unit = json_handler["unit"];
  int wrap_mode_x = json_handler["wrap_mode_x"];
  int wrap_mode_y = json_handler["wrap_mode_y"];

  std::shared_ptr<Texture> new_texture;
  new_texture = std::make_shared<Texture>();
  new_texture->setFilename(std::string(ASSET_DIR) + "/textures/" + filename);
  new_texture->setName(name);
  new_texture->init();
  new_texture->setUnit(unit);
  new_texture->setWrapModes(wrap_mode_x, wrap_mode_y);
  return new_texture;
}

std::shared_ptr<Program> GameRenderer::ProgramFromJSON(std::string filepath) {
  // Read in the file
  std::ifstream json_input_stream(filepath, std::ifstream::in);
  // Read the file into the JSON library
  nlohmann::json json_handler;
  json_input_stream >> json_handler;

  // Get name attributes from JSON
  std::string vert_name = json_handler["vert"];
  std::string frag_name = json_handler["frag"];
  std::string prog_name = json_handler["name"];

  // Create new shader program
  std::shared_ptr<Program> new_program;
  new_program = std::make_shared<Program>();
  new_program->setVerbose(true);
  new_program->setName(prog_name);
  new_program->setShaderNames(std::string(ASSET_DIR) + "/shaders/" + vert_name,
                              std::string(ASSET_DIR) + "/shaders/" + frag_name);
  new_program->init();

  // Create the uniforms
  std::vector<std::string> uniforms = json_handler["uniforms"];
  for (int i = 0; i < uniforms.size(); i++) {
    new_program->addUniform(uniforms[i]);
  }
  // Create the attributes
  std::vector<std::string> attributes = json_handler["attributes"];
  for (int i = 0; i < attributes.size(); i++) {
    new_program->addAttribute(attributes[i]);
  }

  return new_program;
}

std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GameRenderer::GetObjectsInView(std::shared_ptr<std::vector<glm::vec4>> vfplane,
                               std::shared_ptr<Octree> tree) {
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> inView =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
  std::queue<Node*> toVisit;
  toVisit.push(tree->GetRoot());
  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (node->objects->empty()) {
      for (Node* child : *(node->children)) {
        if (!ViewFrustumCulling::IsCulled(child->boundingBox, vfplane)) {
          toVisit.push(child);
        }
      }
    } else {
      for (std::shared_ptr<GameObject> objectInBox : *(node->objects)) {
        if (!ViewFrustumCulling::IsCulled(objectInBox->GetBoundingBox(),
                                          vfplane)) {
          inView->insert(objectInBox);
        }
      }
    }
  }
  return inView;
}

void GameRenderer::Render(GLFWwindow* window,
                          std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Level> level = game_state->GetLevel();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Player> player = game_state->GetPlayer();

  RendererSetup::PreRender(window);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float aspect = width / (float)height;

  auto P = std::make_shared<MatrixStack>();
  auto V = camera->getView();
  MatrixStack MV;

  P->pushMatrix();
  P->perspective(45.0f, aspect, 0.01f, 100.0f);
  V.pushMatrix();

  std::shared_ptr<std::vector<glm::vec4>> vfplane =
      ViewFrustumCulling::GetViewFrustumPlanes(P->topMatrix(), V.topMatrix());

  game_state->SetItemsInView(GetObjectsInView(vfplane, level->getTree()));

  // Platforms
  std::shared_ptr<Program> current_program = programs["platform_prog"];
  std::shared_ptr<Texture> current_texture = textures["lightgrey"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));

  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::PLATFORM) {
      MV = obj->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      obj->GetModel()->draw(current_program);
    }
  }
  current_program->unbind();

  // moving platforms
  current_program = programs["moving_platform_prog"];
  current_texture = textures["skyblue"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::MOVING_PLATFORM ||
        obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM) {
      MV = obj->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      obj->GetModel()->draw(current_program);
    }
  }
  current_program->unbind();

  // Collectible Notes
  current_program = programs["note_prog"];
  current_program->bind();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));

  int color_count = 0;
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::NOTE) {
      if (std::shared_ptr<gameobject::Note> note =
              std::static_pointer_cast<gameobject::Note>(obj)) {
        if (!note->GetCollected()) {
          MV = note->GetTransform();
          glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                             glm::value_ptr(MV.topMatrix()));
          glm::vec3 cur_color = color_vec.at(color_count);
          color_count++;
          if (color_count == 5) {
            color_count = 0;
          }
          glUniform3f(current_program->getUniform("in_obj_color"), cur_color.x,
                      cur_color.y, cur_color.z);
          note->GetModel()->draw(current_program);
        }
      }
    }
  }
  current_program->unbind();

  // Player
  current_program = programs["player_prog"];
  current_texture = textures["rainbowglass"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  MV = player->GetTransform();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                     glm::value_ptr(MV.topMatrix()));
  player->GetModel()->draw(current_program);

  P->popMatrix();
  V.popMatrix();

  ImGuiRender(game_state);

  RendererSetup::PostRender(window);
}

void GameRenderer::ImGuiRender(std::shared_ptr<GameState> game_state) {
  static const ImGuiWindowFlags static_window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;

  ImGui_ImplGlfwGL3_NewFrame();

  ImGui::Begin("Stats", NULL, static_window_flags);

  std::string score_string =
      "Score: " + std::to_string(game_state->GetPlayer()->GetScore());
  ImGui::Text(score_string.c_str());

#ifdef DEBUG
  static double last_debug_time = glfwGetTime();
  static int frames_since_last_debug = 0;
  static std::string fps_string = "FPS: no data";

  double current_debug_time = glfwGetTime();
  double elapsed_debug_time = current_debug_time - last_debug_time;
  if (elapsed_debug_time > 1.0) {
    // more than a second has elapsed, so print an update
    double fps = frames_since_last_debug / elapsed_debug_time;
    std::ostringstream fps_stream;
    fps_stream << std::setprecision(4) << "FPS: " << fps;
    fps_string = fps_stream.str();
    last_debug_time = current_debug_time;
    frames_since_last_debug = 0;
  }
  frames_since_last_debug++;

  ImGui::Text(fps_string.c_str());
#endif

  ImGui::End();

  ImGui::Render();
}
