#include "Shader.h"

Shader::Shader() : m_type(0), m_ID(-1) {}

Shader::~Shader() {
  free( m_source );
}

Shader::Shader( const string& path, GLenum type ) {
  m_type = type;
  read( path );
  compile();
}

GLvoid Shader::read( const string& path ) {
  char* source = textFileRead( path.c_str() );
  m_source = source;
}

GLboolean Shader::compile() {
  m_ID = glCreateShader( m_type );

  const GLchar* derp = m_source;

  glShaderSource( m_ID, 1, &derp, NULL );

  glCompileShader( m_ID );

  return true;
}

GLvoid Shader::printLog() {
  GLint infoLogLength = 0;
  GLint count = 0;
  char* infoLog;

  glGetShaderiv( m_ID, GL_INFO_LOG_LENGTH, &infoLogLength );
  
  if ( infoLogLength > 0 ) {
    infoLog = (GLchar*)malloc(infoLogLength);
    glGetShaderInfoLog( m_ID, infoLogLength, &count, infoLog );
    printf( "%s\n", infoLog );
    free( infoLog );
  }
}