#ifndef LIGHT_H
#define LIGHT_H

#include "Light.h"

using namespace std;

class Spotlight : public Light {
public:
  Spotlight() {
    Light::init();
    init();
  }

  ~Spotlight() {}

  GLvoid init() {
     memset( m_amb, 0, sizeof(m_dir) );
     m_exponent = 0.0f;
     m_cutoff = 0.0f;
     m_constAttenuation = 0.0f;
     m_linearAttenuation = 0.0f;
     m_quadAttenuation = 0.0f;
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

  vec3  m_dir;        //Spotlight direction
  GLfloat m_exponent; // Spotlight attenuation exponent
  GLfloat m_cutoff;   // Spotlight cutoff angle
  GLfloat m_constAttenuation;  // Constant attenuation factor
  GLfloat m_linearAttenuation; // Linear attenuation factor
  GLfloat m_quadAttenuation;   // Quadractic attenuation factor
};

#endif