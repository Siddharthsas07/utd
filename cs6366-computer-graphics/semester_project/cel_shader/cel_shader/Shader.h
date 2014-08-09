#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <iostream>
#include "textfile.h"
#include <string>

using namespace std;

class Shader {
public:
  Shader();
  Shader( const string& path, GLenum type );
  ~Shader();

  GLvoid read( const string& path );

  GLboolean compile();

  GLvoid printLog();

  GLint     m_ID;     // GL-assigned shader ID
  GLchar *  m_source; // 2D array of null terminated strings for shader source code
  GLenum    m_type;   // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
};

#endif