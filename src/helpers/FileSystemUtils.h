// Alex Ottoboni
#ifndef __FILE_SYS_UTILS__
#define __FILE_SYS_UTILS__

#include <iostream>
#include <vector>

class FileSystemUtils {
  public:
    FileSystemUtils();
    ~FileSystemUtils();
    static std::vector<std::string> ListFiles(const std::string& pattern);
};
#endif
