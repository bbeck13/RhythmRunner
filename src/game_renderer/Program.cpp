#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;

Program::Program() :
   vShaderName(""),
   fShaderName(""),
   gShaderName(""),
   pid(0),
   verbose(true) {

}

Program::~Program() {

}

void Program::setShaderNames(const string &v, const string &f) {
   vShaderName = v;
   fShaderName = f;
}

void Program::setShaderNames(const string &v, const string &f, const string &g) {
   vShaderName = v;
   fShaderName = f;
   gShaderName = g;
}

bool Program::init() {
   GLint rc;
   bool isGeomShader = false;
   if (this->gShaderName != "") {
      isGeomShader = true;
   }

   // Create shader handles
   GLuint VS = glCreateShader(GL_VERTEX_SHADER);
   GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
   GLuint GS;
   if (isGeomShader) {
      GS = glCreateShader(GL_GEOMETRY_SHADER);
   }

   // Read shader sources
   const char *vshader = GLSL::textFileRead(vShaderName.c_str());
   const char *fshader = GLSL::textFileRead(fShaderName.c_str());
   const char *gshader;
   glShaderSource(VS, 1, &vshader, NULL);
   glShaderSource(FS, 1, &fshader, NULL);

   if (isGeomShader) {
      gshader = GLSL::textFileRead(gShaderName.c_str());
      glShaderSource(GS, 1, &gshader, NULL); 
   }

   // Compile vertex shader
   glCompileShader(VS);
   glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
   if(!rc) {
      if(isVerbose()) {
         GLSL::printShaderInfoLog(VS);
         cout << "Error compiling vertex shader " << vShaderName << endl;
      }
      return false;
   }

   // Compile fragment shader
   glCompileShader(FS);
   glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
   if(!rc) {
      if(isVerbose()) {
         GLSL::printShaderInfoLog(FS);
         cout << "Error compiling fragment shader " << fShaderName << endl;
      }
      return false;
   }

   if (isGeomShader) {
      // Compile geom shader
      glCompileShader(GS);
      glGetShaderiv(GS, GL_COMPILE_STATUS, &rc);
      if(!rc) {
         if(isVerbose()) {
            GLSL::printShaderInfoLog(GS);
            cout << "Error compiling geom shader " << gShaderName << endl;
         }
         return false;
      }
   }

   // Create the program and link
   pid = glCreateProgram();
   glAttachShader(pid, VS);
   glAttachShader(pid, FS);
   if (isGeomShader) {
      glAttachShader(pid, GS);
   }
   glLinkProgram(pid);
   glGetProgramiv(pid, GL_LINK_STATUS, &rc);
   if(!rc) {
      if(isVerbose()) {
         GLSL::printProgramInfoLog(pid);
         cout << "Error linking shaders " << vShaderName << " and " << fShaderName << endl;
      }
      return false;
   }

   GLSL::checkError(GET_FILE_LINE);
   return true;
}

void Program::bind() {
   glUseProgram(pid);
}

void Program::unbind() {
   glUseProgram(0);
}

void Program::addAttribute(const string &name) {
   attributes[name] = glGetAttribLocation(pid, name.c_str());
}

void Program::addUniform(const string &name) {
   uniforms[name] = glGetUniformLocation(pid, name.c_str());
}

GLint Program::getAttribute(const string &name) const {
   map<string,GLint>::const_iterator attribute = attributes.find(name.c_str());
   if(attribute == attributes.end()) {
      if(isVerbose()) {
         cout << name << " is not an attribute variable" << endl;
      }
      return -1;
   }
   return attribute->second;
}

GLint Program::getUniform(const string &name) const {
   map<string,GLint>::const_iterator uniform = uniforms.find(name.c_str());
   if(uniform == uniforms.end()) {
      if(isVerbose()) {
         cout << name << " is not a uniform variable" << endl;
      }
      return -1;
   }
   return uniform->second;
}

std::string Program::getName() const {
   return progName;
}

void Program::setName(const string &name) {
   progName = name;
}
