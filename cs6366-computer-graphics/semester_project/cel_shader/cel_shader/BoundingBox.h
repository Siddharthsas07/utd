#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "glTypes.h"
#include "useful_things.h"

using namespace std;

class BoundingBox {
public:
  BoundingBox() {
    memset( m_min, 0, sizeof(m_min) );
    memset( m_max, 0, sizeof(m_max) );
  }

  vert3 m_min;
  vert3 m_max;
};

#endif