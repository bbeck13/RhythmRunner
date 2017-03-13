// Alex Ottoboni

#include "GameRenderer.h"

#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <queue>
#include <algorithm>  // std::copy_if, std::distance

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
#include "CollisionCalculator.h"

#define TEXT_FIELD_LENGTH 256
#define SHOW_ME_THE_MENU_ITEMS 4
#define ENDGAME_MENU_WAIT_SECONDS 0.5

namespace {

void DrawPhysicalObjectTree(std::shared_ptr<Program> program,
                            MatrixStack P,
                            MatrixStack V,
                            std::shared_ptr<PhysicalObject> physical_object) {
  std::queue<std::shared_ptr<PhysicalObject>> queue;
  queue.push(physical_object);

  while (!queue.empty()) {
    std::shared_ptr<PhysicalObject> object = queue.front();
    queue.pop();

    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P.topMatrix()));
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE,
                       glm::value_ptr(V.topMatrix()));
    glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE,
                       glm::value_ptr(object->GetTransform()));
    object->GetModel()->draw(program);

    for (std::shared_ptr<PhysicalObject> sub_object : object->GetSubObjects()) {
      queue.push(sub_object);
    }
  }
}
}

GameRenderer::GameRenderer() {}

GameRenderer::~GameRenderer() {}

void GameRenderer::Init(const std::string& resource_dir, GLFWwindow* window) {
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

  programs["bloom_final_prog"]->bind();
  glUniform1i(programs["bloom_final_prog"]->getUniform("scene"), 0);
  glUniform1i(programs["bloom_final_prog"]->getUniform("bloomBlur"), 1);
  programs["bloom_final_prog"]->unbind();

  int width, height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  // hdrFBO stores normal scene and to-be-blurred scene
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glGenTextures(2, hdrColorBuffers);
  for (GLuint i = 0; i < 2; i++)
  {
    glBindTexture(GL_TEXTURE_2D, hdrColorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
                 width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, hdrColorBuffers[i], 0);
  }
  // depth framebuffer
  GLuint rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);
  GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // pingpongFBO and pingpongColorbuffers used for blurring
  glGenFramebuffers(2, pingpongFBO);
  glGenTextures(2, pingpongColorbuffers);
  float tempTexWidth = width / 2;
  float tempTexHeight = height / 2;
  for (GLuint i = 0; i < 2; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, tempTexWidth, tempTexHeight,
                 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
  }
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;


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
  std::string geom_name;
  bool isGeomShader = false;
  if (json_handler.find("geom") != json_handler.end()) {
    geom_name = json_handler["geom"];
    isGeomShader = true;
  }

  // Create new shader program
  std::shared_ptr<Program> new_program;
  new_program = std::make_shared<Program>();
  new_program->setVerbose(true);
  new_program->setName(prog_name);
  
  if (isGeomShader) {
    new_program->setShaderNames(ASSET_DIR "/shaders/" + vert_name,
                                ASSET_DIR "/shaders/" + frag_name,
                                ASSET_DIR "/shaders/" + geom_name);
  } else {
    new_program->setShaderNames(ASSET_DIR "/shaders/" + vert_name,
                                ASSET_DIR "/shaders/" + frag_name);
  }

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

MainProgramMode GameRenderer::Render(GLFWwindow* window,
                                     std::shared_ptr<GameState> game_state) {
  RenderObjects(window, game_state);
  glClear(GL_DEPTH_BUFFER_BIT);
  RenderMinimap(window, game_state);
  MainProgramMode next_mode = ImGuiRenderGame(game_state);
  RendererSetup::PostRender(window);
  return next_mode;
}

LevelProgramMode GameRenderer::RenderLevelEditor(
    GLFWwindow* window,
    std::shared_ptr<GameState> game_state) {
  RenderObjects(window, game_state);
  glClear(GL_DEPTH_BUFFER_BIT);
  RenderMinimap(window, game_state);
  LevelProgramMode next_mode = ImGuiRenderEditor(game_state);
  RendererSetup::PostRender(window);
  return next_mode;
}

void GameRenderer::RenderMinimap(GLFWwindow* window,
                                 std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Level> level = game_state->GetLevel();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<Sky> sky = game_state->GetSky();

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width / 6, height / 6);
  float aspect = width / (float)height;

  auto P = std::make_shared<MatrixStack>();
  GameCamera mini_cam = GameCamera(glm::vec3(camera->getPosition().x, 5, 100),
                                   camera->getLookAt(), camera->getUp());
  mini_cam.Refresh();
  auto V = std::make_shared<MatrixStack>(mini_cam.getView());
  auto MV = std::make_shared<MatrixStack>();

  P->pushMatrix();
  // small far for aggressive culling
  P->perspective(45.0f, aspect, 0.01f, 200.0f);
  V->pushMatrix();

  std::shared_ptr<std::vector<glm::vec4>> vfplane =
      ViewFrustumCulling::GetViewFrustumPlanes(P->topMatrix(), V->topMatrix());

  game_state->SetItemsInView(
      GameRenderer::GetObjectsInView(vfplane, level->getTree()));

  // large far for sexy looks
  P->popMatrix();
  P->pushMatrix();
  P->perspective(20.0f, aspect, 0.01f, 1000.0f);

  player->SetScale(glm::vec3(10, 10, 10));
  RenderSingleObject(player, P, V);
  player->SetScale(glm::vec3(1, 1, 1));

  if (player->GetGround()) {
    RenderSingleObject(player->GetGround(), programs["player_prog"],
                       textures["rainbowass"], P, V);
  }
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::PLATFORM,
                     textures["nightsky"], P, V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::MOVING_PLATFORM, textures["nightsky"], P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::DROPPING_PLATFORM_UP, textures["nightsky"],
                     P, V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::DROPPING_PLATFORM_DOWN,
                     textures["nightsky"], P, V);
  RenderLevelCollectibles(game_state->GetObjectsInView(), SecondaryType::NOTE,
                          P, V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::MONSTER, P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::MOONROCK, P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::PLAINROCK,
                     P, V);
  P->popMatrix();
  V->popMatrix();
}
void GameRenderer::RenderSingleObject(std::shared_ptr<GameObject> object,
                                      std::shared_ptr<Program> program,
                                      std::shared_ptr<Texture> texture,
                                      std::shared_ptr<MatrixStack> P,
                                      std::shared_ptr<MatrixStack> V) {
  program->bind();
  texture->bind(program->getUniform("Texture0"));
  DrawPhysicalObjectTree(program, *P, *V,
                         std::static_pointer_cast<PhysicalObject>(object));
  program->unbind();
}

void GameRenderer::RenderSingleObject(std::shared_ptr<GameObject> object,
                                      std::shared_ptr<MatrixStack> P,
                                      std::shared_ptr<MatrixStack> V) {
  object->GetProgram()->bind();
  object->GetTexture()->bind(object->GetProgram()->getUniform("Texture0"));
  DrawPhysicalObjectTree(object->GetProgram(), *P, *V,
                         std::static_pointer_cast<PhysicalObject>(object));
  object->GetProgram()->unbind();
}

std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>
GameRenderer::GetObjectsOfType(
    std::unordered_set<std::shared_ptr<GameObject>>* objects,
    SecondaryType type_to_get) {
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects_to_render =
      std::make_shared<std::vector<std::shared_ptr<GameObject>>>(
          objects->size());
  auto it =
      std::copy_if(objects->begin(), objects->end(), objects_to_render->begin(),
                   [&](std::shared_ptr<GameObject> obj) {
                     return obj->GetSecondaryType() == type_to_get;
                   });
  objects_to_render->resize(std::distance(objects_to_render->begin(), it));
  return objects_to_render;
}

void GameRenderer::RenderLevelObjects(
    std::unordered_set<std::shared_ptr<GameObject>>* objects,
    SecondaryType type_to_render,
    std::shared_ptr<MatrixStack> P,
    std::shared_ptr<MatrixStack> V) {
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects_to_render =
      GetObjectsOfType(objects, type_to_render);
  if (!objects_to_render->empty()) {
    std::shared_ptr<Program> program = objects_to_render->front()->GetProgram();
    std::shared_ptr<Texture> texture = objects_to_render->front()->GetTexture();
    program->bind();
    texture->bind(program->getUniform("Texture0"));

    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P->topMatrix()));
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE,
                       glm::value_ptr(V->topMatrix()));
    for (std::shared_ptr<GameObject> obj : *objects_to_render) {
      glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(obj->GetTransform()));
      obj->GetModel()->draw(program);
    }
    program->unbind();
  }
}
void GameRenderer::RenderLevelObjects(
    std::unordered_set<std::shared_ptr<GameObject>>* objects,
    SecondaryType type_to_render,
    std::shared_ptr<Texture> video_texture,
    std::shared_ptr<MatrixStack> P,
    std::shared_ptr<MatrixStack> V) {
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects_to_render =
      GetObjectsOfType(objects, type_to_render);
  if (!objects_to_render->empty()) {
    std::shared_ptr<Program> program = objects_to_render->front()->GetProgram();
    std::shared_ptr<Texture> texture = objects_to_render->front()->GetTexture();
    program->bind();
    texture->bind(program->getUniform("Texture0"));
    video_texture->bind(program->getUniform("SkyTexture0"));

    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P->topMatrix()));
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE,
                       glm::value_ptr(V->topMatrix()));
    for (std::shared_ptr<GameObject> obj : *objects_to_render) {
      glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE,
                         glm::value_ptr(obj->GetTransform()));
      obj->GetModel()->draw(program);
    }
    program->unbind();
  }
}

void GameRenderer::RenderLevelCollectibles(
    std::unordered_set<std::shared_ptr<GameObject>>* objects,
    SecondaryType type_to_render,
    std::shared_ptr<MatrixStack> P,
    std::shared_ptr<MatrixStack> V) {
  std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> objects_to_render =
      GetObjectsOfType(objects, type_to_render);
  if (!objects_to_render->empty()) {
    std::shared_ptr<Program> program = objects_to_render->front()->GetProgram();
    program->bind();
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE,
                       glm::value_ptr(P->topMatrix()));
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE,
                       glm::value_ptr(V->topMatrix()));
    int color_count = 0;
    for (std::shared_ptr<GameObject> obj : *objects_to_render) {
      if (std::shared_ptr<Collectible> collectible =
              std::static_pointer_cast<Collectible>(obj)) {
        glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE,
                           glm::value_ptr(collectible->GetTransform()));
        glm::vec3 cur_color = color_vec.at(color_count);
        color_count++;
        if (color_count == 5) {
          color_count = 0;
        }
        glUniform3f(program->getUniform("in_obj_color"), cur_color.x,
                    cur_color.y, cur_color.z);
        glUniform1i(program->getUniform("isCollected"), collectible->GetCollected());
        glUniform1i(program->getUniform("timeCollected"), collectible->GetTicksCollected());
        collectible->GetModel()->draw(program);
      }
    }
    program->unbind();
  }
}

void GameRenderer::RenderObjects(GLFWwindow* window,
                                 std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Level> level = game_state->GetLevel();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<Sky> sky = game_state->GetSky();

  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  float aspect = width / (float)height;
  auto P = std::make_shared<MatrixStack>();
  auto V = std::make_shared<MatrixStack>(camera->getView());
  auto MV = std::make_shared<MatrixStack>();


  P->pushMatrix();
  // small far for aggressive culling
  P->perspective(45.0f, aspect, 0.01f, 150.0f);
  V->pushMatrix();

  std::shared_ptr<std::vector<glm::vec4>> vfplane =
      ViewFrustumCulling::GetViewFrustumPlanes(P->topMatrix(), V->topMatrix());

  game_state->SetItemsInView(
      GameRenderer::GetObjectsInView(vfplane, level->getTree()));

  // large far for sexy looks
  P->popMatrix();
  P->pushMatrix();
  P->perspective(45.0f, aspect, 0.01f, 1000.0f);

  RenderSingleObject(player, P, V);
  if (player->GetGround()) {
    RenderSingleObject(player->GetGround(), programs["current_platform_prog"],
                       textures["rainbowass"], P, V);
  }
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::PLATFORM,
                     textures["nightsky"], P, V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::MOVING_PLATFORM, textures["nightsky"], P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::DROPPING_PLATFORM_UP, textures["nightsky"],
                     P, V);
  RenderLevelObjects(game_state->GetObjectsInView(),
                     SecondaryType::DROPPING_PLATFORM_DOWN,
                     textures["nightsky"], P, V);
  RenderLevelCollectibles(game_state->GetObjectsInView(), SecondaryType::NOTE,
                          P, V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::MONSTER, P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::MOONROCK, P,
                     V);
  RenderLevelObjects(game_state->GetObjectsInView(), SecondaryType::PLAINROCK,
                     P, V);
  RenderSingleObject(sky, P, V);

  P->popMatrix();
  V->popMatrix();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // blur the brightColor scene using blur fragment shader
  programs["blur_prog"]->bind();

  GLboolean horizontal = true, first_iteration = true;
  GLuint amount = 8;
  glViewport(0, 0, width / 2.0, height / 2.0);
  for (GLuint i = 0; i < amount; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
    glUniform1i(programs["blur_prog"]->getUniform("horizontal"), horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? hdrColorBuffers[1] :
                  pingpongColorbuffers[!horizontal]);
    RenderQuad();
    horizontal = !horizontal;
    if (first_iteration)
      first_iteration = false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  programs["blur_prog"]->unbind();

  // combine bloom and normal scenes
  glViewport(0, 0, width, height);
  GLboolean bloom = true;
  GLfloat exposure = 1.2f;
  programs["bloom_final_prog"]->bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdrColorBuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[horizontal]);
  glUniform1i(programs["bloom_final_prog"]->getUniform("bloom"), bloom);
  glUniform1f(programs["bloom_final_prog"]->getUniform("exposure"), exposure);
  RenderQuad();
  programs["bloom_final_prog"]->unbind();
}

GLuint quadVAO = 0;
GLuint quadVBO;
void GameRenderer::RenderQuad()
{
  if (quadVAO == 0)
  {
    GLfloat quadVertices[] = {
      // Positions        // Texture Coords
      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
                 &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void GameRenderer::ImGuiRenderBegin(std::shared_ptr<GameState> game_state) {
  ImGui_ImplGlfwGL3_NewFrame();

#ifdef DEBUG
  RendererSetup::ImGuiTopRightCornerWindow(0.2, RendererSetup::STATIC);
  ImGui::Begin("Debug Info", NULL, RendererSetup::STATIC_WINDOW_FLAGS);

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
  ImGui::Text(
      (std::string("anim: ") +
       Player::AnimationToString(game_state->GetPlayer()->GetAnimation()))
          .c_str());

  ImGui::End();
#endif
}

void GameRenderer::ImGuiRenderEnd() {
  ImGui::Render();
}

MainProgramMode GameRenderer::ImGuiRenderGame(
    std::shared_ptr<GameState> game_state) {
  MainProgramMode next_mode = MainProgramMode::GAME_SCREEN;

  ImGuiRenderBegin(game_state);

  RendererSetup::ImGuiTopLeftCornerWindow(0.2, RendererSetup::STATIC);
  ImGui::Begin("Stats", NULL, RendererSetup::STATIC_WINDOW_FLAGS);

  std::string score_string =
      "Score: " + std::to_string(game_state->GetPlayer()->GetScore());
  ImGui::Text(score_string.c_str());

  std::string progress_string =
      std::string("Progress: ") +
      std::to_string((int)(game_state->GetProgressRatio() * 100.0)) + "%%";
  ImGui::Text(progress_string.c_str());

  ImGui::End();

  GameState::PlayingState playing_state = game_state->GetPlayingState();
  switch (playing_state) {
    case GameState::PlayingState::PLAYING:
      break;
    case GameState::PlayingState::PAUSED:
      RendererSetup::ImGuiCenterWindow(0.5, RendererSetup::DYNAMIC);
      ImGui::Begin("PAUSED", NULL, RendererSetup::DYNAMIC_WINDOW_FLAGS);
      ImGui::Text("Paused");
      if (ImGui::Button("Resume [ESCAPE]") ||
          InputBindings::KeyPressed(GLFW_KEY_ESCAPE)) {
        game_state->SetPlayingState(GameState::PlayingState::PLAYING);
      }
      if (ImGui::Button("Main Menu [ENTER]") ||
          InputBindings::KeyPressed(GLFW_KEY_ENTER)) {
        next_mode = MainProgramMode::MENU_SCREEN;
      }
      if (ImGui::Button("Exit [Q]") || InputBindings::KeyPressed(GLFW_KEY_Q)) {
        next_mode = MainProgramMode::EXIT;
      }
      ImGui::End();
      break;
    case GameState::PlayingState::FAILURE:
    case GameState::PlayingState::SUCCESS: {
      // TODO(jarhar): make screen green/red or something
      // TODO(jarhar): insert particle/sound effects here
      // TODO(jarhar): rotate camera around player here
      bool success = playing_state == GameState::PlayingState::SUCCESS;
      if (glfwGetTime() >
          game_state->GetEndingTime() + ENDGAME_MENU_WAIT_SECONDS) {
        RendererSetup::ImGuiCenterWindow(0.5, RendererSetup::DYNAMIC);
        ImGui::Begin(success ? "SUCCESS" : "FAILURE", NULL,
                     RendererSetup::DYNAMIC_WINDOW_FLAGS);
        ImGui::Text(success ? "YOU WIN!" : "YOU FAILED");
        ImGui::Text(score_string.c_str());
        ImGui::Text(progress_string.c_str());
        if (ImGui::Button("Retry [SPACE]") ||
            InputBindings::KeyPressed(GLFW_KEY_SPACE)) {
          next_mode = MainProgramMode::RESET_GAME;
        }
        if (ImGui::Button("Main Menu [ENTER]") ||
            InputBindings::KeyPressed(GLFW_KEY_ENTER)) {
          next_mode = MainProgramMode::MENU_SCREEN;
        }
        if (ImGui::Button("Exit [ESCAPE]") ||
            InputBindings::KeyPressed(GLFW_KEY_ESCAPE)) {
          next_mode = MainProgramMode::EXIT;
        }
        ImGui::End();
      } else {
        // This makes sure that when the end ui shows up,
        // buffered keypresses are not used
        InputBindings::ClearKeyPresses();
      }
      break;
    }
  }

  ImGuiRenderEnd();
  return next_mode;
}

LevelProgramMode GameRenderer::ImGuiRenderEditor(
    std::shared_ptr<GameState> game_state) {
  ImGuiRenderBegin(game_state);

  std::shared_ptr<LevelEditorState> level_state =
      game_state->GetLevelEditorState();
  RendererSetup::ImGuiCenterWindow(0.3, RendererSetup::DYNAMIC);
  ImGui::Begin("Level Editor", NULL, RendererSetup::DYNAMIC_WINDOW_FLAGS);
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
            pos, scale, rotation_angle, rotation_axis));
        break;
      case 5:
        game_state->GetLevel()->AddItem(std::make_shared<gameobject::MoonRock>(
            pos, scale, rotation_angle, rotation_axis));
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
        colliding_objs = CollisionCalculator::GetCollidingObjects(
            game_state->GetPlayer()->GetBoundingBox(),
            game_state->GetLevel()->getTree());

    if (!colliding_objs->empty()) {
      game_state->GetLevel()->RemoveItem(*colliding_objs->begin());
    }
  }

  ImGui::End();

  ImGui::Begin("Export Level", NULL, RendererSetup::DYNAMIC_WINDOW_FLAGS);
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
  ImGui::Begin("Camera Sensitivity", NULL, RendererSetup::DYNAMIC_WINDOW_FLAGS);
  std::string camera_move_string =
      "Movement: " +
      std::to_string(game_state->GetLevelEditorState()->GetCameraMove());
  std::string camera_move_yaw_string =
      "Yaw: " +  // tick yaw
      std::to_string(game_state->GetLevelEditorState()->GetCameraYawMove());
  ImGui::Text(camera_move_string.c_str());
  ImGui::Text(camera_move_yaw_string.c_str());
  ImGui::End();

  RendererSetup::ImGuiBottomRightCornerWindow(0.15, RendererSetup::STATIC);
  ImGui::Begin("Help", NULL, RendererSetup::STATIC_WINDOW_FLAGS);
  ImGui::Text("To toggle cursor");
  ImGui::Text("press [ESCAPE]");
  ImGui::End();

  ImGuiRenderEnd();

  return LevelProgramMode::EDIT_LEVEL;  // TODO(jarhar): make use of this
}
