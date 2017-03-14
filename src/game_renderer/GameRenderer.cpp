// Alex Ottoboni

#include "GameRenderer.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <queue>

#include "FileSystemUtils.h"
#include "InputBindings.h"
#include "LevelGenerator.h"
#include "MatrixStack.h"
#include "MovingPlatform.h"
#include "Octree.h"
#include "Platform.h"
#include "Note.h"
#include "DroppingPlatform.h"
#include "ViewFrustumCulling.h"
#include "json.hpp"
#include "RendererSetup.h"
#include "Sky.h"
#include "MoonRock.h"
#include "PlainRock.h"
#include "Monster.h"
#include "LevelJson.h"
#include "GameUpdater.h"
#include "Particles.h"

#define TEXT_FIELD_LENGTH 256
#define SHOW_ME_THE_MENU_ITEMS 4

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

void GameRenderer::Init(const std::string& resource_dir) {
  glClearColor(.2f, .2f, .2f, 1.0f);
  // Initialize all programs from JSON files in assets folder
  std::shared_ptr<Program> temp_program;
  std::vector<std::string> json_files =
      FileSystemUtils::ListFiles(ASSET_DIR "/shaders", "*.json");
  for (int i = 0; i < json_files.size(); i++) {
    temp_program = GameRenderer::ProgramFromJSON(json_files[i]);
    programs[temp_program->getName()] = temp_program;
  }

  std::shared_ptr<Texture> temp_texture;
  std::vector<std::string> texture_files =
      FileSystemUtils::ListFiles(ASSET_DIR "/textures", "*.json");
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
  new_program->setShaderNames(ASSET_DIR "/shaders/" + vert_name,
                              ASSET_DIR "/shaders/" + frag_name);
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

std::unordered_set<std::shared_ptr<GameObject>>* GameRenderer::GetObjectsInView(
    std::shared_ptr<std::vector<glm::vec4>> vfplane,
    std::shared_ptr<Octree> tree) {
  std::unordered_set<std::shared_ptr<GameObject>>* inView =
      new std::unordered_set<std::shared_ptr<GameObject>>();
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
  RenderIt(window, game_state, false);
}
void GameRenderer::RenderLevelEditor(GLFWwindow* window,
                                     std::shared_ptr<GameState> game_state) {
  RenderIt(window, game_state, true);
}

void GameRenderer::RenderIt(GLFWwindow* window,
                            std::shared_ptr<GameState> game_state,
                            bool render_level_editor) {
  std::shared_ptr<Level> level = game_state->GetLevel();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<Sky> sky = game_state->GetSky();
  Particles = new ParticleGenerator(5000);
  RendererSetup::PreRender(window);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float aspect = width / (float)height;

  auto P = std::make_shared<MatrixStack>();
  auto V = camera->getView();
  MatrixStack MV;

  P->pushMatrix();
  // small far for aggressive culling
  P->perspective(45.0f, aspect, 0.01f, 150.0f);
  V.pushMatrix();

  std::shared_ptr<std::vector<glm::vec4>> vfplane =
      ViewFrustumCulling::GetViewFrustumPlanes(P->topMatrix(), V.topMatrix());

  game_state->SetItemsInView(
      GameRenderer::GetObjectsInView(vfplane, level->getTree()));

  // large far for sexy looks
  P->popMatrix();
  P->pushMatrix();
  P->perspective(45.0f, aspect, 0.01f, 1000.0f);

  // Ground
  std::shared_ptr<Program> current_program;
  std::shared_ptr<Texture> current_texture;
  if (player->GetGround()) {
    current_program = programs["player_prog"];
    current_texture = textures["rainbowass"];
    current_program->bind();
    current_texture->bind(current_program->getUniform("Texture0"));
    MV = player->GetGround()->GetTransform();
    glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P->topMatrix()));
    glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                       glm::value_ptr(V.topMatrix()));
    glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                       glm::value_ptr(MV.topMatrix()));
    player->GetGround()->GetModel()->draw(current_program);
    current_program->unbind();
  }

  // Platforms
  current_program = programs["platform_prog"];
  current_texture = textures["lunarrock"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  textures["nightsky"]->bind(current_program->getUniform("SkyTexture0"));
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
  current_texture = textures["lunarrock"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  textures["nightsky"]->bind(current_program->getUniform("SkyTexture0"));
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

  // Monsters
  current_program = programs["monster_prog"];
  current_texture = textures["rainbowass"];
  current_program->bind();
  current_texture->bind(current_program->getUniform("Texture0"));
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::MONSTER) {
      MV = obj->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      obj->GetModel()->draw(current_program);
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
  current_program->unbind();

  // Sky
  current_program = programs["sky_prog"];
  current_program->bind();
  current_texture = textures["nightsky"];
  current_texture->bind(current_program->getUniform("Texture0"));
  MV = sky->GetTransform();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                     glm::value_ptr(MV.topMatrix()));
  sky->GetModel()->draw(current_program);
  current_program->unbind();

  // Moon Rocks
  current_program = programs["rock_prog"];
  current_program->bind();
  current_texture = textures["rock"];
  current_texture->bind(current_program->getUniform("Texture0"));
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::MOONROCK) {
      if (std::shared_ptr<gameobject::MoonRock> rock =
              std::static_pointer_cast<gameobject::MoonRock>(obj)) {
        if (obj != player->GetGround()) {
          MV = rock->GetTransform();
          glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                             glm::value_ptr(MV.topMatrix()));
          rock->GetModel()->draw(current_program);
        }
      }
    }
  }
  current_program->unbind();

  // Plain Rocks
  current_program = programs["rock_prog"];
  current_program->bind();
  current_texture = textures["rock"];
  current_texture->bind(current_program->getUniform("Texture0"));
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (obj->GetSecondaryType() == SecondaryType::PLAINROCK) {
      if (std::shared_ptr<gameobject::PlainRock> rock =
              std::static_pointer_cast<gameobject::PlainRock>(obj)) {
        MV = rock->GetTransform();
        glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                           glm::value_ptr(MV.topMatrix()));
        rock->GetModel()->draw(current_program);
      }
    }
  }
  current_program->unbind();
    
    // Particles
    Particles->Update(2, game_state->GetPlayer(), glm::vec3(0.0f, 0.0f, 0.0f));
    current_program = programs["particle_prog"];
    current_program->bind();
    current_texture = textures["particletex"];
    current_texture->bind(current_program->getUniform("Texture0"));
    glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P->topMatrix()));
    Particles->DrawParticles(current_program);
    current_program->unbind();
    
  // Sky
  current_program = programs["sky_prog"];
  current_program->bind();
  current_texture = textures["nightsky"];
  current_texture->bind(current_program->getUniform("Texture0"));
  MV = sky->GetTransform();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                     glm::value_ptr(MV.topMatrix()));
  sky->GetModel()->draw(current_program);

  P->popMatrix();
  V.popMatrix();

  ImGuiRender(game_state, render_level_editor);

  RendererSetup::PostRender(window);
}

void GameRenderer::ImGuiRender(std::shared_ptr<GameState> game_state,
                               bool level_editor) {
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

  if (level_editor) {
    LevelEditorRenderer(game_state);
  }

  ImGui::Render();
}

void GameRenderer::LevelEditorRenderer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<LevelEditorState> level_state =
      game_state->GetLevelEditorState();
  RendererSetup::ImGuiCenterWindow(0.3);
  ImGui::Begin("Level Editor");
  glm::vec3 pos = game_state->GetPlayer()->GetPosition();
  std::string look_at = "Position:{" + std::to_string(pos.x) + ", " +
                        std::to_string(pos.y) + ", " + std::to_string(pos.z) +
                        "}";

  ImGui::Text(look_at.c_str());
  const char* game_object_types[] = {
      "Platform",  "MovingPlatform", "Note",   "DroppingPlatform",
      "PlainRock", "MoonRock",       "Monster"};
  static int listbox_item_current = -1;
  // TODO Snake Plisken is there a way to do number of members of a c array
  ImGui::ListBox("##game_object_select", &listbox_item_current,
                 game_object_types, 7, SHOW_ME_THE_MENU_ITEMS);
  static glm::vec3 scale;
  static glm::vec3 rotation_axis;
  static glm::vec3 p1;
  static glm::vec3 p2;
  static float rotation_angle;
  static float drop_vel;
  static float distanceX;
  static float distanceZ;
  switch (listbox_item_current) {
    case 0: {  // Platform
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.5f);
      ImGui::InputFloat("Y", &scale.y, 0.5f);
      ImGui::InputFloat("Z", &scale.z, 0.5f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      break;
    }
    case 1: {  // moving platform
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.5f);
      ImGui::InputFloat("Y", &scale.y, 0.5f);
      ImGui::InputFloat("Z", &scale.z, 0.5f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      ImGui::Text(std::string("Path").c_str());
      ImGui::InputFloat("Point 1 +X", &p1.x, 0.1f);
      ImGui::InputFloat("Point 1 +Y", &p1.y, 0.1f);
      ImGui::InputFloat("Point 1 +Z", &p1.z, 0.1f);
      ImGui::InputFloat("Point 2 +X", &p2.x, 0.1f);
      ImGui::InputFloat("Point 2 +Y", &p2.y, 0.1f);
      ImGui::InputFloat("Point 2 +Z", &p2.z, 0.1f);
      ImGui::InputFloat("Velocity", &drop_vel, 0.05f);
      break;
    }
    case 2: {  // Note
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.2f);
      ImGui::InputFloat("Y", &scale.y, 0.2f);
      ImGui::InputFloat("Z", &scale.z, 0.2f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      break;
    }
    case 3: {  // dropping platform
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.5f);
      ImGui::InputFloat("Y", &scale.y, 0.5f);
      ImGui::InputFloat("Z", &scale.z, 0.5f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      ImGui::InputFloat("Drop Velocity", &drop_vel, 0.05f);
      break;
    }
    case 4: {  // plain rock
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.2f);
      ImGui::InputFloat("Y", &scale.y, 0.2f);
      ImGui::InputFloat("Z", &scale.z, 0.2f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      break;
    }
    case 5: {  // moon rock
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.2f);
      ImGui::InputFloat("Y", &scale.y, 0.2f);
      ImGui::InputFloat("Z", &scale.z, 0.2f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      break;
    }
    case 6: {  // monster
      ImGui::Text(std::string("Scale").c_str());
      ImGui::InputFloat("X", &scale.x, 0.2f);
      ImGui::InputFloat("Y", &scale.y, 0.2f);
      ImGui::InputFloat("Z", &scale.z, 0.2f);
      ImGui::Text(std::string("Rotation Axis").c_str());
      ImGui::InputFloat("X Axis", &rotation_axis.x, 0.2f);
      ImGui::InputFloat("Y Axis", &rotation_axis.y, 0.2f);
      ImGui::InputFloat("Z Axis", &rotation_axis.z, 0.2f);
      ImGui::InputFloat("Rotation Angle (radians)", &rotation_angle, 0.2f);
      ImGui::InputFloat("Velocity", &drop_vel, 0.05f);
      ImGui::InputFloat("Travel X", &distanceX, 0.05f);
      ImGui::InputFloat("Travel Z", &distanceZ, 0.05f);
    }
    default:
      break;
  }

  if (ImGui::Button("Add")) {
    switch (listbox_item_current) {
      case 0:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::Platform>(
            pos, scale, rotation_axis, rotation_angle));
        break;
      case 1: {
        std::vector<glm::vec3> path = std::vector<glm::vec3>();
        // move in a triangle
        path.push_back(pos + p1);
        path.push_back(pos + p1 + p2);
        path.push_back(pos);
        game_state->GetLevel()->AddItem(
            std::make_shared<gameobject::MovingPlatform>(
                pos, scale, rotation_axis, rotation_angle,
                glm::vec3(drop_vel, drop_vel, drop_vel), path));
        break;
      }
      case 2:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::Note>(
            pos, scale, rotation_axis, rotation_angle, false));
        break;
      case 3:
        game_state->GetLevel()->AddItem(
            std::make_shared<gameobject::DroppingPlatform>(
                pos, scale, rotation_axis, rotation_angle, drop_vel, false));
        break;
      case 4:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::PlainRock>(
            pos, scale, rotation_axis, rotation_angle));
        break;
      case 5:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::MoonRock>(
            pos, scale, rotation_axis, rotation_angle));
        break;
      case 6:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::Monster>(
            pos, scale, rotation_axis, rotation_angle,
            glm::vec3(drop_vel, drop_vel, drop_vel), distanceX, distanceZ));

      default:
        break;
    }
  }
  if (ImGui::Button("Remove")) {
    std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
        colliding_objs = GameUpdater::GetCollidingObjects(
            game_state->GetPlayer()->GetBoundingBox(),
            game_state->GetLevel()->getTree());

    if (!colliding_objs->empty()) {
      game_state->GetLevel()->getTree()->remove(*colliding_objs->begin());
    }
  }

  ImGui::End();

  ImGui::Begin("Export Level");
  static char level_path_buffer[TEXT_FIELD_LENGTH];

  strncpy(level_path_buffer, level_state->GetLevelPath().c_str(),
          TEXT_FIELD_LENGTH);
  if (ImGui::InputText("Level Filepath", level_path_buffer,
                       TEXT_FIELD_LENGTH)) {
    level_state->SetLevelPath(
        std::string(level_path_buffer, TEXT_FIELD_LENGTH));
  }
  if (ImGui::Button("Save")) {
    if (!level_state->GetLevelPath().empty()) {
      nlohmann::json j = *game_state->GetLevel()->getObjects();
      std::ofstream output(level_state->GetLevelPath());
      output << j;
    }
  }
  if (ImGui::Button("Refresh Tree")) {
    game_state->GetLevel()->SetTree(
        std::make_shared<Octree>(game_state->GetLevel()->getObjects()));
  }
  ImGui::End();
  ImGui::Begin("Camera Sensitivity");
  std::string camera_move_string =
      "Movement: " +
      std::to_string(game_state->GetLevelEditorState()->GetCameraMove());
  std::string camera_move_yaw_string =
      "Yaw: " +  // tick yaw
      std::to_string(game_state->GetLevelEditorState()->GetCameraYawMove());
  ImGui::Text(camera_move_string.c_str());
  ImGui::Text(camera_move_yaw_string.c_str());
  ImGui::End();
}
