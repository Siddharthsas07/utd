#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include "Shader.h"

using namespace std;

class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram( const string& vert, const string& frag );
  ~ShaderProgram();

  GLvoid read( const string& vert, const string& frag );

  GLboolean compile();

  GLvoid printLog();

  GLint     m_ID;    // GL-assigned program ID
  Shader    m_vert;  // Vertex shader
  Shader    m_frag;  // Fragment shader
};

#endif