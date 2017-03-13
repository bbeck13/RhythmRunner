// Alex Ottoboni

#include "FileSystemUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

#define TEMP_DIR ASSET_DIR "/temp"

#ifdef _WIN32

#include <windows.h>
#include <urlmon.h>
#define EXECUTABLE_DIR ASSET_DIR "/executables/windows"

#else  // _WIN32

#include <glob.h>
#define EXECUTABLE_DIR (ASSET_DIR "/executables/mac")

#endif  // _WIN32

namespace FileSystemUtils {

// Uses glob to list all files in path with pattern matching
std::vector<std::string> ListFiles(const std::string& path,
                                   const std::string& pattern) {
  std::string path_and_pattern = path + "/" + pattern;
  std::vector<std::string> files;
#ifdef _WIN32
  WIN32_FIND_DATA find_file_data;
  HANDLE find_handle = FindFirstFile(path_and_pattern.c_str(), &find_file_data);
  if (find_handle == INVALID_HANDLE_VALUE) {
    return files;
  }
  bool success;
  do {
    files.push_back(path + "/" + std::string(find_file_data.cFileName));
    success = FindNextFile(find_handle, &find_file_data);
  } while (success);
  FindClose(find_handle);
#else
  glob_t glob_result;
  glob(path_and_pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
  for (int i = 0; i < glob_result.gl_pathc; ++i) {
    files.push_back(std::string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
#endif
  return files;
}

bool FileExists(const std::string& path) {
  return std::ifstream(path).good();
}

int DownloadYoutubeVideo(const std::string& url) {
  // TODO(jarhar): use popen()/CreateProcess() to capture output from system()
  // TODO(jarhar): figure out if video was already downloaded?
  // TODO(jarhar): figure out how to accomodate 30fps and 60fps videos

  int system_return;
  std::stringstream command_stream;

#ifdef _WIN32
  command_stream << "cd " TEMP_DIR " & " EXECUTABLE_DIR "/youtube-dl \"" << url
                 << "\" -o video.mp4 & " EXECUTABLE_DIR
                    "/ffmpeg -i video.mp4 image-%05d.jpg";
#else   // _WIN32
  command_stream << "cd " TEMP_DIR " && export PATH=$PATH:" EXECUTABLE_DIR
                    " && " EXECUTABLE_DIR "/youtube-dl \""
                 << url << "\" -o video.mp4 && " EXECUTABLE_DIR
                           "/ffmpeg -i video.mp4 image-%05d.jpg";
#endif  // _WIN32
  system_return = system(command_stream.str().c_str());
#ifdef DEBUG
  std::cout << __FUNCTION__ << " system() returned: " << system_return
            << std::endl;
#endif  // DEBUG
  return system_return;
}
}
