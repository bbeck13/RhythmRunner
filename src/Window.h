#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <map>
#include <functional>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "MatrixStack.h"
#include "Camera.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class Window {
public:
   Window(int height, int width, string title);
   ~Window();
   static void resize(int width, int height);
   static void resizeCallback(GLFWwindow *window, int width, int height);
   static bool shouldClose();
   static void setClose(bool toSet);
   static void swapBuffers();
   static float getAspect();
   static int getHeight();
   static int getWidth();
   static void cursorCallback(GLFWwindow *window, double x, double y);
   static MatrixStack getView();

private:
   static bool close;
   static int height;
   static int width;
   static GLFWwindow *window;
   static Camera *camera;
};

#endif
