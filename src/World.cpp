#include "World.h"

int World::GiboLen;
GLuint World::GrndBuffObj;
GLuint World::GrndNorBuffObj;
GLuint World::GrndTexBuffObj;
GLuint World::GIndxBuffObj;

shared_ptr<Program> prog2;
shared_ptr<Texture> texture2;

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
   glClearColor(0.30f, 0.5f, 0.78f, 1.0f);

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

   prog2 = make_shared<Program>();
   prog2->setVerbose(true);
   prog2->setShaderNames("../assets/shaders/tex_vert.glsl", "../assets/shaders/tex_frag2.glsl");
   prog2->init();

   texture2 = make_shared<Texture>();
   texture2->setFilename("../assets/textures/grass.jpg");
   texture2->init();
   texture2->setUnit(2);
   texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

   prog2->addUniform("P");
   prog2->addUniform("MV");
   prog2->addAttribute("vertPos");
   prog2->addAttribute("vertNor");
   prog2->addAttribute("vertTex");
   prog2->addUniform("Texture2");

   GlobalData::insertCallback(GLFW_KEY_ESCAPE, [] (int scancode, int action, int mods) {
      if (action == GLFW_PRESS) {
         Window::setClose(true);
      }
   });
}

void World::update() {
   while (!window->shouldClose()) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      auto P = MatrixStack();
      auto MV = MatrixStack();
      P.pushMatrix();
      P.perspective(45.0f, window->getAspect(), 0.01f, 100.0f);

      prog2->bind();
      MV.pushMatrix();
      texture2->bind(prog2->getUniform("Texture2"));
      glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, value_ptr(P.topMatrix()));
      glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, value_ptr(MV.topMatrix()));

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

      // draw!
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      glDrawElements(GL_TRIANGLES, GiboLen, GL_UNSIGNED_SHORT, 0);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);

      MV.popMatrix();
      prog2->unbind();

      P.popMatrix();

      window->swapBuffers();
      glfwPollEvents();
      if (glGetError() != GL_NO_ERROR) {
         window->setClose(true);
      }
   }
}
