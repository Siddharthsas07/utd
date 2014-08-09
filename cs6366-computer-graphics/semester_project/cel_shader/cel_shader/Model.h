#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <cstdio>
#include <cfloat>
#include "glTypes.h"
#include "Triangle.h"
#include "Material.h"
#include "BoundingBox.h"
#include "useful_things.h"

using namespace std;

class Model {
public:
  Model();
  Model( const string& path );
  ~Model();

  // Read in the model data from file located at path
  void read( const string& path );  

  void print();

  BoundingBox      m_box;    // Bounding box: [xmin,ymin,zmin,xmax,ymax,zmax]
  vec3             m_center; // Geometric center
  vector<Triangle> m_tris;   // Trinagles that make this model
  vector<Material> m_mats;   // Materials that make this model
};

#endif