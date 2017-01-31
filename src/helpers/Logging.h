// Joseph Arhar

#ifndef LOGGING_H_
#define LOGGING_H_

// TODO(jarhar): figure out why DEBUG flag isnt working
//#ifdef DEBUG
//#define LOG(message) std::cout << "[" << __FILE__ << " " << __FUNCTION__ << "] " << message << std::endl;
#define LOG(message) std::cout << __FUNCTION__ << " " << message << std::endl;
//#else
//#define LOG(message) do {} while (0)
//#endif  // DEBUG

#endif  // LOGGING_H_
