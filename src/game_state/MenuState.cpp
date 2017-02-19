// Joseph Arhar

#include "MenuState.h"
#include <iostream>
#include <string>
#include <stdio.h>

MenuState::MenuState() {}

MenuState::~MenuState() {}

static bool isLoaded = false;

std::string MenuState::GetMusicPath() {
  return music_path;
}

const char* GetYoutubeId(const char* url) {
  return strstr(url, "=") + 1;
}

std::string MenuState::GetYoutubeVideo(std::string url) {
  if (isLoaded == true) {
    return music_path;
  }
  std::string youtube_id = GetYoutubeId(url.c_str());
  std::cout << youtube_id << std::endl;
  std::string youtube_dl_command = "youtube-dl --id --restrict-filenames --write-info-json -k --prefer-free-formats " + url;
  std::system(youtube_dl_command.c_str());
  std::string ffmpeg_command_audio = "ffmpeg -i " + youtube_id + ".*.webm -qscale 0 " + youtube_id + ".wav";
  std::string ffmpeg_command_video = "ffmpeg -i " + youtube_id + ".mkv ../assets/textures/image-%d.jpg";
  std::system(ffmpeg_command_audio.c_str());
  std::system(ffmpeg_command_video.c_str());
  std::cout << youtube_id << std::endl;
  isLoaded = true;
  return youtube_id + ".wav";
}

void MenuState::SetMusicPath(std::string music_path) {
  this->music_path = music_path;
}
