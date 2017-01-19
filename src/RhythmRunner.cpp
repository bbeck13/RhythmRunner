/**
 * RhythmRunner.cpp
 *
 *
 * For now this is just a bunch of import statements to make assert the dependencies
 * Braden Beck (bnbeck)
 *
 */

/*cpp includes*/
#include <iostream>

/* Aquila stuff*/
#include "aquila/global.h"
#include "aquila/source/WaveFile.h"
#include "aquila/tools/TextPlot.h"
#include "aquila/transform/AquilaFft.h"
#include "aquila/transform/OouraFft.h"
#include "aquila/source/generator/SineGenerator.h"
#include "aquila/transform/FftFactory.h"

/*SFML*/
#include <SFML/Audio.hpp>

/*GL stuff glew glfw etc..*/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"


// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define RESOURCE_DIR "../assets/"
#define RESOURCE_ARG 1

using namespace std;

int main(int argc, char **argv) {
   string resource_dir;

   if (argc < 2) {
      cout << "Looking for resources in " << RESOURCE_DIR << endl;
      resource_dir = RESOURCE_DIR;
   } else {
      resource_dir = argv[RESOURCE_ARG];
   }

   if (resource_dir.back() != '/') {
      resource_dir.append("/");
   }

   cout << "RhythmRunner" << endl;
   return EXIT_SUCCESS;
}
