// Joseph Arhar

#ifndef MODEL_RENDERER_H_
#define MODEL_RENDERER_H_

#include <map>
#include <memory>

#include "game_state/Model.h"
#include "game_renderer/Program.h"

struct ModelRenderData;

class ModelRenderer {
 public:
  ModelRenderer();
  ~ModelRenderer();

  void InitModel(std::shared_ptr<Model> model);
  void DrawModel(std::shared_ptr<Program> prog, std::shared_ptr<Model> model);
 
 private:
  std::map<std::shared_ptr<Model>, ModelRenderData> model_to_data;
};

#endif
