#ifndef MATERIAL_H
#define MATERIAL_H

#include "glTypes.h"

class Material {
public:
  Material() {
    memset( m_ambient, 0, sizeof(m_ambient) );
    memset( m_diffuse, 0, sizeof(m_diffuse) );
    memset( m_specular, 0, sizeof(m_specular) );
    m_shininess = 0;
  }

  col4    m_ambient;
  col4    m_diffuse;
  col4    m_specular;  
  GLfloat m_shininess;
};

#endif