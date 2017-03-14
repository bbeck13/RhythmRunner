#pragma  once
#ifndef __Program__
#define __Program__

#include <map>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

class Program {
public:
   Program();
   virtual ~Program();

   void setVerbose(bool v) {
      verbose = v;
   }
   bool isVerbose() const {
      return verbose;
   }

   void setShaderNames(const std::string &v, const std::string &f);
   void setShaderNames(const std::string &v, const std::string &f, const std::string &g);
   virtual bool init();
   virtual void bind();
   virtual void unbind();

   void addAttribute(const std::string &name);
   void addUniform(const std::string &name);
   GLint getAttribute(const std::string &name) const;
   GLint getUniform(const std::string &name) const;
   std::string getName() const;
   void setName(const std::string &name);

protected:
   std::string vShaderName;
   std::string fShaderName;
   std::string gShaderName;
   std::string progName;

private:
   GLuint pid;
   std::map<std::string,GLint> attributes;
   std::map<std::string,GLint> uniforms;
   bool verbose;
};

#endif
