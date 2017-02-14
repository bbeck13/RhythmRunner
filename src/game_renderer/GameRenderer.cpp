// Alex Ottoboni

#include "GameRenderer.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <queue>

#include "GLSL.h"
#include "GL/glew.h"
#include "json.hpp"
#include "MatrixStack.h"
#include "FileSystemUtils.h"
#include "LevelGenerator.h"
#include "Platform.h"
#include "MovingPlatform.h"
#include "InputBindings.h"
#include "ViewFrustumCulling.h"
#include "Octree.h"

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

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

void GameRenderer::Init(const std::string& resource_dir) {
  // OpenGL Setup Boilerplate
  if (!glfwInit()) {
    std::cerr << "!glfwInit()" << std::endl;
    exit(1);
  }
  // request the highest possible version of OGL - important for mac
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  window = glfwCreateWindow(mode->width, mode->height, TITLE, monitor, NULL);
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glewExperimental = true;

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    exit(1);
  }
  glGetError();  // weird bootstrap of glGetError
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
  glfwSwapInterval(1);  // vsync

  GLSL::checkVersion();
  glClearColor(.12f, .34f, .56f, 1.0f);  // set background color
  glEnable(GL_DEPTH_TEST);               // enable z-buffer test
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize all programs from JSON files in assets folder
  std::shared_ptr<Program> temp_program;
  std::vector<std::string> json_files =
      FileSystemUtils::ListFiles(std::string(ASSET_DIR) + "/shaders", "*.json");
  for (int i = 0; i < json_files.size(); i++) {
    temp_program = GameRenderer::ProgramFromJSON(json_files[i]);
    programs[temp_program->getName()] = temp_program;
  }

  ImGuiInit();
}

void GameRenderer::InitState(std::shared_ptr<GameState> state) {
  InputBindings::Init(state, this->window);
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

void GameRenderer::Render(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Level> level = game_state->GetLevel();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Player> player = game_state->GetPlayer();

  int width, height;
  glfwGetFramebufferSize(this->window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
  current_program->bind();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));

  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (std::shared_ptr<Platform> platform =
            std::dynamic_pointer_cast<Platform>(obj)) {
      MV = platform->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      platform->GetModel()->draw(current_program);
    } else if (std::shared_ptr<MovingPlatform> platform =
            std::dynamic_pointer_cast<MovingPlatform>(obj)) {
      MV = platform->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      platform->GetModel()->draw(current_program);
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

  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    if (std::shared_ptr<Note> note = std::dynamic_pointer_cast<Note>(obj)) {
      if (!note->GetCollected()) {
        MV = note->GetTransform();
        glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                           glm::value_ptr(MV.topMatrix()));
        note->GetModel()->draw(current_program);
      }
    }
  }
  current_program->unbind();

  // Player
  current_program = programs["player_prog"];
  current_program->bind();
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

  glfwSwapBuffers(this->window);
  glfwPollEvents();
  if (game_state->Done()) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

void GameRenderer::Close() {
  InputBindings::Close();
  glfwDestroyWindow(window);
  glfwTerminate();
}

bool GameRenderer::WindowShouldClose() {
  return glfwWindowShouldClose(window);
}

void GameRenderer::ImGuiInit() {
  ImGui_ImplGlfwGL3_Init(window, false);  // false -> don't use glfw keybindings
}

void GameRenderer::ImGuiRender(std::shared_ptr<GameState> game_state) {
  static const ImGuiWindowFlags static_window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;

  ImGui_ImplGlfwGL3_NewFrame();

  ImGui::Begin("Stats", NULL, static_window_flags);

  std::string score_string =
      "Score: " + std::to_string(game_state->GetPlayer()->GetScore());
  ImGui::Text(score_string.c_str());

#ifdef DEBUG  // Print fps and stuff
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
