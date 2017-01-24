#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "Renderer.h"
#include "GameObject.h"

class Graphics {
public:
   Graphics();
   virtual void init(GameObject *gameObject);
   virtual void render(GameObject *gameObject);
private:
   Renderer renderer;
};
#endif /*__GRAPHICS_H__*/
