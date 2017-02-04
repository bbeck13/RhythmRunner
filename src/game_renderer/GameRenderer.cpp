// Alex Ottoboni

#include "GameRenderer.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "GL/glew.h"
#include "json.hpp"
#include "MatrixStack.h"
#include "FileSystemUtils.h"
#include "LevelGenerator.h"
#include "Platform.h"
#include "InputBindings.h"

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

GLFWwindow* GameRenderer::GetWindow() {
  return window;
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

void GameRenderer::Init(const std::string& resource_dir,
                        std::shared_ptr<GameState> state) {
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
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE, NULL, NULL);
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
  glfwSwapInterval(0);  // vsync

  InputBindings::Init(state, window);

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
}

#define LEFT 0
#define RIGHT 1
#define BOTTOM 2
#define TOP 3
#define NEAR 4
#define FAR 5

vec4 * GetViewFrustumPlanes(mat4 P, mat4 V) {
  vec4 Left, Right, Bottom, Top, Near, Far;
  vec4 *planes = (vec4 *)malloc(sizeof(vec4) * 6);
  vec3 normal;
  float normal_length;
 
  mat4 comp = P * V;
  
  Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
  Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
  Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
  Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
  normal = vec3(Left.x, Left.y, Left.z);
  normal_length = glm::length(normal);
  planes[0] = Left/normal_length;
  Left = Left/normal_length;

  Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
  Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
  Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
  Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
  normal = vec3(Right.x, Right.y, Right.z);
  normal_length = glm::length(normal);
  planes[1] = Right/normal_length;
  Right = Right/normal_length;

  Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
  Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
  Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
  Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp
  normal = vec3(Bottom.x, Bottom.y, Bottom.z);
  normal_length = glm::length(normal);
  planes[2] = Bottom/normal_length;
  Bottom = Bottom/normal_length;

  Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
  Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
  Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
  Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
  normal = vec3(Top.x, Top.y, Top.z);
  normal_length = glm::length(normal);
  planes[3] = Top/normal_length;
  Top = Top/normal_length;

  Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
  Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
  Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
  Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
  normal = vec3(Near.x, Near.y, Near.z);
  normal_length = glm::length(normal);
  planes[4] = Near/normal_length;
  Near = Near/normal_length;

  Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
  Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
  Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
  Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
  normal = vec3(Far.x, Far.y, Far.z);
  normal_length = glm::length(normal);
  planes[5] = Far/normal_length;
  Far = Far/normal_length;

  return planes;
}

float DistToPlane(float A, float B, float C, float D, vec3 point) {
  return (A * point.x + B * point.y + C * point.z + D);
}

bool IsCulled(AxisAlignedBox box, vec4 * planes) {
  float dist;
  // If the max point is on the left side of the plane, cull
  dist = DistToPlane(planes[LEFT].x, planes[LEFT].y, planes[LEFT].z, planes[LEFT].w, box.GetMax());
  if (dist <= 0) {
    return true;
  }

  // If the min point is on the right side of the plane, cull
  dist = DistToPlane(planes[RIGHT].x, planes[RIGHT].y, planes[RIGHT].z, planes[RIGHT].w, box.GetMin());
  if (dist <= 0) {
    return true;
  }

  // If the min point is above the plane, cull
  dist = DistToPlane(planes[TOP].x, planes[TOP].y, planes[TOP].z, planes[TOP].w, box.GetMin());
  if (dist <= 0) {
    return true;
  }

  // If the max point is below the plane, cull
  dist = DistToPlane(planes[BOTTOM].x, planes[BOTTOM].y, planes[BOTTOM].z, planes[BOTTOM].w, box.GetMax());
  if (dist <= 0) {
    return true;
  }

  // If the max point is in front of the plane, cull
  dist = DistToPlane(planes[NEAR].x, planes[NEAR].y, planes[NEAR].z, planes[NEAR].w, box.GetMax());
  if (dist <= 0) {
    return true;
  }

  // If the min point is in behind the plane, cull
  dist = DistToPlane(planes[FAR].x, planes[FAR].y, planes[FAR].z, planes[FAR].w, box.GetMax());
  if (dist <= 0) {
    return true;
  }

  return false;
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

  vec4 *vfplane = GetViewFrustumPlanes(P->topMatrix(), V.topMatrix());

  std::shared_ptr<Program> current_program = programs["platform_prog"];
  current_program->bind();
  glUniformMatrix4fv(current_program->getUniform("P"), 1, GL_FALSE,
                     glm::value_ptr(P->topMatrix()));
  glUniformMatrix4fv(current_program->getUniform("V"), 1, GL_FALSE,
                     glm::value_ptr(V.topMatrix()));

  for (std::shared_ptr<Platform> platform : *level->getLevel()) {
    if (!IsCulled(platform->GetBoundingBox(), vfplane)) {
      MV = platform->GetTransform();
      glUniformMatrix4fv(current_program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(MV.topMatrix()));
      platform->GetModel()->draw(current_program);
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
