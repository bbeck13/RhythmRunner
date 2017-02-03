// Joseph Arhar

#ifndef LOGGING_H_
#define LOGGING_H_

#include <iostream>

#define _LOG(message) std::cout << __FUNCTION__ << " " << message << std::endl;
#define _LOG_DUMMY(message) do {} while (0)

#define LOG_ERROR(message) std::cerr << __FUNCTION__ << " " << message << std::endl;

#ifdef _WIN32

#ifdef _DEBUG
#define LOG(message) _LOG(message)
#else  // _DEBUG
#define LOG(message) _LOG_DUMMY(message)
#endif  // _DEBUG

#else  // _WIN32

#ifdef DEBUG
#define LOG(message) _LOG(message)
#else  // DEBUG
#define LOG(message) _LOG_DUMMY(message)
#endif  // DEBUG

#endif  // _WIN32

#endif  // LOGGING_H_
