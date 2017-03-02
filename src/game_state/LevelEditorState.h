#ifndef LEVEL_EDITOR_STATE_H_
#define LEVEL_EDITOR_STATE_H_

#include <string>

class LevelEditorState {
 public:
  LevelEditorState();
  ~LevelEditorState();
  std::string GetLevelPath();
  void SetLevelPath(std::string level_path);
  float GetCameraMove();
  float GetCameraYawMove();

  void SetCameraMove(float move);
  void SetCameraYawMove(float move);

 private:
  float camera_yaw_move;
  float camera_move;
  std::string level_path;
};
#endif
