// Alex Ottoboni
#ifndef __FILE_SYS_UTILS__
#define __FILE_SYS_UTILS__

#include <vector>
#include <string>

namespace FileSystemUtils {

std::vector<std::string> ListFiles(const std::string& path,
                                   const std::string& pattern);
bool FileExists(const std::string& path);

int DownloadYoutubeVideo(const std::string& url);
}

#endif
