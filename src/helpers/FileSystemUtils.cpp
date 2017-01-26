// Alex Ottoboni

#include <FileSystemUtils.h>
#include <glob.h>

// Uses glob to list all files in path with pattern matching
std::vector<std::string> FileSystemUtils::ListFiles(const std::string& pattern) {
  glob_t glob_result;
  glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
  std::vector<std::string> files;
  for (int i = 0; i < glob_result.gl_pathc;++i) {
    files.push_back(std::string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
  return files;
}

