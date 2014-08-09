#ifndef COLORIZER_H
#define COLORIZER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
#include "glTypes.h"
#include "useful_things.h"

using namespace std;

class Colorizer {
private:
  GLvoid init();
  GLfloat COLOR_CHANGE;

public:
  Colorizer();
  Colorizer( col3f* colors );
  Colorizer( col4f* colors );
  ~Colorizer();

  const GLfloat operator[]( GLuint index ) const;
  GLfloat& operator[]( GLuint index ); 
  GLfloat get( GLuint index ) const;

  // Setting of color (overloads)
  GLvoid setColor( col3f* c );
  GLvoid setColor( col4f* c );  
  GLvoid setColor( GLfloat r, GLfloat g, GLfloat b );
  GLvoid setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );

  // Change color value (r,g,b,a) in dir direction(pos/neg)
  GLvoid changeColor( GLchar color, GLbyte dir );
  GLvoid addColor( GLchar color, GLfloat amount );

  // Print statics about scene to console
  GLvoid print();

  col4f   m_color;  // Current color for drawing
  GLfloat m_delta;  // Increment/decrement value for default color changes
};

#endif