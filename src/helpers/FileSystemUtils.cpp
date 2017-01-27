// Alex Ottoboni

#include <FileSystemUtils.h>

#ifdef _WIN32
#else
#include <glob.h>
#endif

// Uses glob to list all files in path with pattern matching
std::vector<std::string> FileSystemUtils::ListFiles(const std::string& pattern) {
  std::vector<std::string> files;
#ifdef _WIN32
  // TODO(jarhar): implement this for windows
#else
  glob_t glob_result;
  glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
  for (int i = 0; i < glob_result.gl_pathc;++i) {
    files.push_back(std::string(glob_result.gl_pathv[i]));
  }
  globfree(&glob_result);
#endif
  return files;
}
