#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glTypes.h"
#include <iostream>

using namespace std;

class Light {
public:
  Light() {
    init();
  }

  ~Light() {}

  GLvoid init() { 
    memset( m_amb,  0, sizeof(m_amb) );
    memset( m_spec, 0, sizeof(m_spec) );
    memset( m_dif,  0, sizeof(m_dif) );
    memset( m_pos,  0, sizeof(m_pos) );
  }

  GLvoid setAmbient( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
    m_amb[0] = r; m_amb[1] = g; m_amb[2] = b; m_amb[3] = a;
  }

  GLvoid setSpecular( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
    m_spec[0] = r; m_spec[1] = g; m_spec[2] = b; m_spec[3] = a;
  } 

  GLvoid setDiffuse( GLfloat r, GLfloat g, GLfloat b, GLfloat a ) {
    m_dif[0] = r; m_dif[1] = g; m_dif[2] = b; m_dif[3] = a;
  } 

  GLvoid setPosition( GLfloat x, GLfloat y, GLfloat z ) {
    m_pos[0] = x; m_pos[1] = y; m_pos[2] = z;
  } 

  GLvoid print() {
    cout << "Light Ambient = " << m_amb[0] << " " << m_amb[1] << " " << m_amb[2] << " " << m_amb[3] << endl;
    cout << "Light Diffuse = " << m_dif[0] << " " << m_dif[1] << " " << m_dif[2] << " " << m_dif[3] << endl;
    cout << "Light Specular = " << m_spec[0] << " " << m_spec[1] << " " << m_spec[2] << " " << m_spec[3] << endl;
    cout << "Light Position = " << m_pos[0] << " " << m_pos[1] << " " << m_pos[2] << endl;
  }

  col4  m_amb;  // Ambient color
  col4  m_spec; // Specular color
  col4  m_dif;  // Diffuse color
  vert3 m_pos;  // Position
};

#endif