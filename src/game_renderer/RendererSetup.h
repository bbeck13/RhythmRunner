// Joseph Arhar

#ifndef RENDERER_SETUP_H_
#define RENDERER_SETUP_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TITLE "Rhythm Runner"

namespace RendererSetup {

GLFWwindow* InitOpenGL();
void Close(GLFWwindow* window);

}

#endif
