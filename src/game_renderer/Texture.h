#pragma once
#ifndef __Texture__
#define __Texture__

#define GLEW_STATIC
#include <GL/glew.h>

#include <string>

class Texture
{
public:
  Texture();
  virtual ~Texture();
  void setFilename(const std::string &f) { filename = f; }
  void setName(const std::string &f) {name = f; }
  void init();
  void setUnit(GLint u) { unit = u; }
  GLint getUnit() const { return unit; }
  void bind(GLint handle);
  void unbind();
  void setWrapModes(GLint wrapS, GLint wrapT); // Must be called after init()
  std::string getName();
	
private:
  std::string filename;
  int width;
  int height;
  GLint unit;
  GLuint tid;
  std::string name;
	
};

#endif
