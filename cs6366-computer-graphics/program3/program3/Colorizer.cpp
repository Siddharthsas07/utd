#include "Colorizer.h"

Colorizer::Colorizer() {
  init();
}

Colorizer::Colorizer( col3f* colors ) { 
  init();
  setColor( colors ); 
}

Colorizer::Colorizer( col4f* colors ) { 
  init();
  setColor( colors ); 
}

GLvoid Colorizer::init() {
  COLOR_CHANGE = 0.05f;
  m_delta = COLOR_CHANGE;
  memset( m_color, 0, arraySize(m_color) );
}

Colorizer::~Colorizer() {}

const GLfloat Colorizer::operator[]( GLuint index ) const {
  return m_color[index];
}

GLfloat& Colorizer::operator[]( GLuint index ) {
  return m_color[index];
}

GLfloat Colorizer::get( GLuint index ) const {
  return m_color[index];
}

GLvoid Colorizer::setColor( col3f* c ) {
  setColor( *c[0], *c[1], *c[2], 1.0 );
}

GLvoid Colorizer::setColor( col4f* c ) {
  setColor( *c[0], *c[1], *c[2], *c[2] );
}

GLvoid Colorizer::setColor( GLfloat r, GLfloat g, GLfloat b ) {
  setColor( r, g, b, 1.0 );
}

GLvoid Colorizer::setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
  m_color[0] = r;  m_color[1] = g;  m_color[2] = b;  m_color[3] = a;
}

GLvoid Colorizer::changeColor( GLchar color, GLbyte dir ) {
  addColor( color, (GLfloat)dir * m_delta );
}

GLvoid Colorizer::addColor( GLchar color, GLfloat amount ) {
  if ( color == 'r' ) { 
    m_color[0] += amount; 
    if ( m_color[0] > 1.0f ) { m_color[0] = 1.0f; }
    else if ( m_color[0] < 0.0f ) { m_color[0] = 0.0f; }
  }

  else if ( color == 'g' ) { 
    m_color[1] += amount; 
    if ( m_color[1] > 1.0f ) { m_color[1] = 1.0f; }  
    else if ( m_color[1] < 0.0f ) { m_color[1] = 0.0f; }
  }

  else if ( color == 'b' ) { 
    m_color[2] += amount;  
    if ( m_color[2] > 1.0f ) { m_color[2] = 1.0f; }  
    else if ( m_color[2] < 0.0f ) { m_color[2] = 0.0f; }
  }

  else if ( color == 'a' ) { 
    m_color[3] += amount; 
    if ( m_color[3] > 1.0f ) { m_color[3] = 1.0f; }  
    else if ( m_color[3] < 0.0f ) { m_color[3] = 0.0f; }
  }
}

GLvoid Colorizer::print() {
  printf( "Current color: %f %f %f %f\n", 
    m_color[0], m_color[1], m_color[2], m_color[3] );
}
