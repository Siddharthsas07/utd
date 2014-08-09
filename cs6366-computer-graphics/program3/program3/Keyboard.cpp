#include "Keyboard.h"

Keyboard::Keyboard() {
  init();
}

GLvoid Keyboard::init() {
  m_keys = new GLboolean[256];
  m_specialKeys = new GLboolean[256];
  memset( m_keys, GL_FALSE, 256 );
  memset( m_specialKeys, GL_FALSE, 256 );
}

Keyboard::~Keyboard() {
  delete [] m_keys;
  delete [] m_specialKeys;
}

GLvoid Keyboard::press( GLubyte key, GLint x, GLint y) { m_keys[key] = GL_TRUE; }
GLvoid Keyboard::release( GLubyte key, GLint x, GLint y) { m_keys[key] = GL_FALSE; }
GLvoid Keyboard::specialPress( GLint key, GLint x, GLint y) { m_specialKeys[key] = GL_TRUE; }
GLvoid Keyboard::specialRelease( GLint key, GLint x, GLint y) { m_specialKeys[key] = GL_FALSE; }

GLboolean Keyboard::isPressed( GLubyte key ) { return m_keys[key]; }
GLboolean Keyboard::isSpecialPressed( GLint key ) { return m_specialKeys[key]; }

GLvoid Keyboard::print() {
  printf( "Normal keys:\n" );
  for ( GLuint i = 0; i < 256; i++ ) {
    printf( "[%u] %i ", i, m_keys[i] );
  }
  printf("\n");

  printf( "Special keys:\n" );
  for ( GLuint i = 0; i < 256; i++ ) {
    printf( "[%u] %i ", i, m_specialKeys[i] );
  }
  printf("\n");
}
