#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include "glTypes.h"

struct Triangle {
  vert3f m_verts[3];   // Array of vertices, x=[0],y=[1],z=[2]
  vec3f  m_norms[3];   // Array of vectors for vertex norms
  vec3f  m_faceNorm;   // Vector for face norm
  col3u  m_colorIndex; // Color index. Not used
};

#endif