// Joseph Arhar

#ifndef LOGGING_H_
#define LOGGING_H_

#include <iostream>

// TODO(jarhar): figure out why DEBUG flag isnt working on windows
#ifdef _WIN32

#define LOG(message) std::cout << __FUNCTION__ << " " << message << std::endl;

#else

#ifdef DEBUG
#define LOG(message) std::cout << __FUNCTION__ << " " << message << std::endl;
#else
#define LOG(message) do {} while (0)
#endif  // DEBUG

#endif  // _WIN32

#endif  // LOGGING_H_
