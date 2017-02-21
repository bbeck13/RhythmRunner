#include "VideoTexture.h"
#include "FileSystemUtils.h"
#include "GameRenderer.h"
#include <string>
#include <iostream>

VideoTexture::VideoTexture() {

}

VideoTexture::~VideoTexture() {

}

VideoTexture::VideoTexture(std::string folder_path) {
  //std::cout << folder_path << std::endl;
  std::vector<std::string> texture_files = 
    FileSystemUtils::ListFiles(std::string(folder_path), "*.json");
  for (int i = 0; i < texture_files.size(); i++) {
    textures.push_back(GameRenderer::TextureFromJSON(texture_files[i]));
  }
  cur_frame = 0;
}

std::shared_ptr<Texture> VideoTexture::GetCurFrame() {
  return textures.at(cur_frame);
}

void VideoTexture::IncrementFrame() {
  if (cur_frame + 1 == textures.size()) {
    cur_frame = cur_frame;
  } else {
    cur_frame++;
  }
}

void VideoTexture::ResetFrameCount() {
  cur_frame = 0;
}
