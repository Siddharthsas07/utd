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
  Colorizer( const col3& colors );
  Colorizer( const col4& colors );
  ~Colorizer();

  const GLfloat operator[]( GLuint index ) const;
  GLfloat& operator[]( GLuint index ); 
  GLfloat get( GLuint index ) const;
  GLfloat r() const;
  GLfloat g() const;
  GLfloat b() const;
  GLfloat a() const;

  // Setting of color (overloads)
  GLvoid setColor( const col3& c );
  GLvoid setColor( const col4& c );  
  GLvoid setColor( GLfloat r, GLfloat g, GLfloat b );
  GLvoid setColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );

  // Change color value (r,g,b,a) in dir direction(pos/neg)
  GLvoid changeColor( GLchar color, GLbyte dir );
  GLvoid addColor( GLchar color, GLfloat amount );

  // Print color to console
  GLvoid print();

  col4    m_color;  // Current color for drawing
  GLfloat m_delta;  // Increment/decrement value for default color changes
};

#endif