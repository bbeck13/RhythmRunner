#include "World.h"

int World::GiboLen;
GLuint World::GrndBuffObj, World::GrndNorBuffObj, World::GrndTexBuffObj, World::GIndxBuffObj;

World::World(Window *window) {
   this->window = window;
}

void World::setClearColor(vec4 clearColor) {
   glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
}

World::~World() {
   delete window;
}

void World::init() {
   GLSL::checkVersion();
   glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
   glEnable(GL_DEPTH_TEST);

   //gameObjects.add();
   float g_groundSize = 20;
   float g_groundY = -1.5;

   // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
   float GrndPos[] = {
      -g_groundSize, g_groundY, -g_groundSize,
      -g_groundSize, g_groundY,  g_groundSize,
      g_groundSize, g_groundY,  g_groundSize,
      g_groundSize, g_groundY, -g_groundSize
   };

   float GrndNorm[] = {
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0,
      0, 1, 0
   };

   static GLfloat GrndTex[] = {
      0, 0, // back
      0, 1,
      1, 1,
      1, 0
   };

   unsigned short idx[] = {0, 1, 2, 0, 2, 3};

   GLuint VertexArrayID;
   //generate the VAO
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);

   GiboLen = 6;
   glGenBuffers(1, &GrndBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

   glGenBuffers(1, &GrndNorBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

   glGenBuffers(1, &GrndTexBuffObj);
   glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

   glGenBuffers(1, &GIndxBuffObj);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

   GlobalData::insertCallback(GLFW_KEY_ESCAPE, [] (int scancode, int action, int mods) {
      if (action == GLFW_PRESS) {
         Window::setClose(true);
      }
   });
}

void World::update() {
   while (!window->shouldClose()) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      window->swapBuffers();
      glfwPollEvents();
   }
}
