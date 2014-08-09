#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : m_ID(-1) {}

ShaderProgram::~ShaderProgram() {}

ShaderProgram::ShaderProgram( const string& vert, const string& frag ) {
  read( vert, frag );

  compile();
}

GLvoid ShaderProgram::read( const string& vert, const string& frag ) {
  m_vert.m_type = GL_VERTEX_SHADER;
  m_vert.read( vert );

  m_frag.m_type = GL_FRAGMENT_SHADER;
  m_frag.read( frag );
}

GLboolean ShaderProgram::compile() {
  m_vert.compile();
  m_frag.compile();

  m_ID = glCreateProgram();
  glAttachShader( m_ID, m_vert.m_ID );
  glAttachShader( m_ID, m_frag.m_ID );

  glLinkProgram( m_ID );

  glUseProgram( m_ID );

  return true;
}

GLvoid ShaderProgram::printLog() {
  GLint infoLogLength = 0;
  GLint count = 0;
  char* infoLog;

  glGetProgramiv( m_ID, GL_INFO_LOG_LENGTH, &infoLogLength );

  if ( infoLogLength > 0 ) { 
    infoLog = (GLchar*)malloc(infoLogLength);
    glGetProgramInfoLog( m_ID, infoLogLength, &count, infoLog );
    printf( "%s\n", infoLog );
    free( infoLog );
  }
}