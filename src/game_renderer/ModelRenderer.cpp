// Joseph Arhar

#include "game_renderer/ModelRenderer.h"

#include <assert.h>
#include <iostream>

#include "game_renderer/GLSL.h"
#include "game_renderer/Program.h"

struct ModelRenderData {
  ModelRenderData()
      : element_id(0), position_id(0), normal_id(0), texture_id(0), vao_id(0) {}
  unsigned element_id;
  unsigned position_id;
  unsigned normal_id;
  unsigned texture_id;
  unsigned vao_id;
};

ModelRenderer::ModelRenderer() {}

ModelRenderer::~ModelRenderer() {}

void ModelRenderer::InitModel(std::shared_ptr<Model> model) {
  // TODO(jarhar): consider checking model_to_data to see if InitModel() was
  // already called
  ModelRenderData model_render_data;
  std::vector<unsigned>& elements = model->GetElements();
  std::vector<float>& positions = model->GetPositions();
  std::vector<float>& normals = model->GetNormals();
  std::vector<float>& texture_coordinates = model->GetTextureCoordinates();

  // Initialize the vertex array object
  glGenVertexArrays(1, &model_render_data.vao_id);
  glBindVertexArray(model_render_data.vao_id);

  // Send the position array to the GPU
  glGenBuffers(1, &model_render_data.position_id);
  glBindBuffer(GL_ARRAY_BUFFER, model_render_data.position_id);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0],
               GL_STATIC_DRAW);

  // Send the normal array to the GPU
  if (normals.empty()) {
    model_render_data.normal_id = 0;
  } else {
    glGenBuffers(1, &model_render_data.normal_id);
    glBindBuffer(GL_ARRAY_BUFFER, model_render_data.normal_id);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0],
                 GL_STATIC_DRAW);
  }

  // Send the texture array to the GPU
  if (texture_coordinates.empty()) {
    model_render_data.texture_id = 0;
  } else {
    glGenBuffers(1, &model_render_data.texture_id);
    glBindBuffer(GL_ARRAY_BUFFER, model_render_data.texture_id);
    glBufferData(GL_ARRAY_BUFFER, texture_coordinates.size() * sizeof(float), &texture_coordinates[0],
                 GL_STATIC_DRAW);
  }

  // Send the element array to the GPU
  glGenBuffers(1, &model_render_data.element_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_render_data.element_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int),
               &elements[0], GL_STATIC_DRAW);

  // Unbind the arrays
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  assert(glGetError() == GL_NO_ERROR);

  model_to_data[model] = model_render_data;
}

void ModelRenderer::DrawModel(std::shared_ptr<Program> prog,
                              std::shared_ptr<Model> model) {
  auto iterator = model_to_data.find(model);
  if (iterator == model_to_data.end()) {
    std::cerr << "Model \"" << model->GetFilename() << "\" not initialized."
              << " Did you call ModelRenderer::InitModel() for this model?"
              << std::endl;
    return;
  }
  ModelRenderData model_render_data = iterator->second;

  int h_pos, h_nor, h_tex;
  h_pos = h_nor = h_tex = -1;

  glBindVertexArray(model_render_data.vao_id);
  // Bind position buffer
  h_pos = prog->getAttribute("vertPos");
  GLSL::enableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, model_render_data.position_id);
  glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

  // Bind normal buffer
  h_nor = prog->getAttribute("vertNor");
  if (h_nor != -1 && model_render_data.normal_id != 0) {
    GLSL::enableVertexAttribArray(h_nor);
    glBindBuffer(GL_ARRAY_BUFFER, model_render_data.normal_id);
    glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);
  }

  if (model_render_data.texture_id != 0) {
    // Bind texcoords buffer
    h_tex = prog->getAttribute("vertTex");
    if (h_tex != -1 && model_render_data.texture_id != 0) {
      GLSL::enableVertexAttribArray(h_tex);
      glBindBuffer(GL_ARRAY_BUFFER, model_render_data.texture_id);
      glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
    }
  }

  // Bind element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_render_data.element_id);

  // Draw
  glDrawElements(GL_TRIANGLES, (int)model->GetElements().size(), GL_UNSIGNED_INT,
                 (const void*)0);

  // Disable and unbind
  if (h_tex != -1) {
    GLSL::disableVertexAttribArray(h_tex);
  }
  if (h_nor != -1) {
    GLSL::disableVertexAttribArray(h_nor);
  }
  GLSL::disableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
