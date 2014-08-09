#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <cstdio>
#include <cfloat>
#include "glTypes.h"
#include "Triangle.h"
#include "useful_things.h"

using namespace std;

class Model {
public:
  Model();
  ~Model();

  // Read in the model data from file located at path
  void read( const string& path );  

  void print();

  GLfloat (*m_ambient)[3];  // Ambient colors
  GLfloat (*m_diffuse)[3];  // Diffuse colors
  GLfloat (*m_specular)[3]; // Specular...something
  GLfloat *m_shine;         // Shininess
  GLfloat *m_box;           // Bounding box: [xmin,ymin,zmin,xmax,ymax,zmax]
  GLfloat *m_center;        // Center of bounding box [x,y,z]

  // Number of rows for ambient, diffuse, specular matrices. 
  // Also number of entries in shine.
  GLuint m_materialCount;

  vector<Triangle> m_tris;  // Trinagles that make this model
};

#endif