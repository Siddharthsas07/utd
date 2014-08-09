#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>
#include "useful_things.h"

using namespace std;

class Keyboard {
private:
  GLvoid init();

public:
  Keyboard();
  ~Keyboard();

  GLvoid press( GLubyte key, GLint x, GLint y);
  GLvoid release( GLubyte key, GLint x, GLint y );  
  GLvoid specialPress( GLint key, GLint x, GLint y );
  GLvoid specialRelease( GLint key, GLint x, GLint y );

  GLboolean isPressed( GLubyte key );
  GLboolean isSpecialPressed( GLint key );

  // Print statics about scene to console
  GLvoid print();

  GLboolean* m_keys;        // Normal key states
  GLboolean* m_specialKeys; // Special key states
};

#endif