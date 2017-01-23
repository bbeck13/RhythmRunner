#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "Renderer.h"

class Graphics {
public:
   Graphics(Renderer *renderer);

private:
   Renderer *renderer;
};
#endif /*__GRAPHICS_H__*/
