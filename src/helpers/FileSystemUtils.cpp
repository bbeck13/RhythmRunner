// Alex Ottoboni

#include "FileSystemUtils.h"

#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#else
#include <glob.h>
#endif

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
}
