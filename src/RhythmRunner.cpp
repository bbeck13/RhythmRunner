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

#include "game_state/GameState.h"

using namespace std;

int main(int argc, char **argv) {
   cout << "RhythmRunner" << endl;

   std::unique_ptr<GameState> game_state;

   return EXIT_SUCCESS;
}
