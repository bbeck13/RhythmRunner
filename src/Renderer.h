#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <string>

#include "Camera.h"

using namespace std;

class Renderer {
public:
   Renderer(string shaderdir);

private:
   string shaderdir;
};

#endif /*__RENDERER_H__*/
