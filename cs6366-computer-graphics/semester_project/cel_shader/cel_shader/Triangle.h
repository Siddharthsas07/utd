#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <vector>
#include "glTypes.h"

struct Triangle {
  vert3 m_verts[3];  // Array of vertices, x=[0],y=[1],z=[2]
  vec3  m_norms[3];  // Array of vectors for vertex norms
  vec3  m_faceNorm;  // Vector for face norm
  col3u m_matIdx;    // Color index, used in Model.
};

#endif