#pragma once
#ifndef __VIDEO_TEXTURE__
#define __VIDEO_TEXTURE__

#include "Texture.h"
#include <memory>
#include <vector>

class VideoTexture
{
public:
  VideoTexture();
  VideoTexture(std::string folder_path);
  std::shared_ptr<Texture> GetCurFrame();
  void IncrementFrame();
  void ResetFrameCount();
  virtual ~VideoTexture();

private:
  std::vector<std::shared_ptr<Texture>> textures;
  int cur_frame;
};
#endif
