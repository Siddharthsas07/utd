#include "Model.h"

using namespace std;

Model::Model() {
 // Nothing. Model will take no memory when first instantiated.
}

Model::Model( const string& path ) {
  read( path );
}

Model::~Model() {}

void Model::read( const string& path ) {
  // Allocate memory for the bounding box
  //m_box = new GLfloat[6];

  // General index for looping.
  //GLuint i, numTriangles, colorIndex[3];
  GLbyte fileIterator;
  FILE* inFile = fopen( path.c_str(), "r" );

  if ( inFile == NULL ) {
    printf( "Error: Unable to read model file %s\n", path.c_str() );
    exit( 1 );
  }

  fscanf( inFile, "%c", &fileIterator );
  while ( fileIterator != '\n' ) {
    fscanf( inFile, "%c", &fileIterator );
  }

  GLuint numTriangles;
  GLuint numMats;

  fscanf( inFile, "# triangles = %d\n", &numTriangles );
  fscanf( inFile, "Material count = %d\n", &numMats );

  // Read materials
  for ( GLuint i = 0; i < numMats; i++ ) {
    Material m;
    fscanf( inFile, "ambient color %f %f %f\n", &(m.m_ambient[0]), &(m.m_ambient[1]), &(m.m_ambient[2]) );
    fscanf( inFile, "diffuse color %f %f %f\n", &(m.m_diffuse[0]), &(m.m_diffuse[1]), &(m.m_diffuse[2]) );
    fscanf( inFile, "specular color %f %f %f\n", &(m.m_specular[0]), &(m.m_specular[1]), &(m.m_specular[2]) );
    fscanf( inFile, "material shine %f\n", &(m.m_shininess) );
    m_mats.push_back( m );
  }

  fscanf( inFile, "%c", &fileIterator );
  while ( fileIterator != '\n' ) {
    fscanf( inFile, "%c", &fileIterator );
  }

  // Allocate and populate Triangles.
  m_tris.resize( numTriangles );
  for ( GLuint i = 0; i < numTriangles; i++ ) {
    fscanf( inFile, "v0 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[0][0]), &(m_tris[i].m_verts[0][1]), &(m_tris[i].m_verts[0][2]),
      &(m_tris[i].m_norms[0][0]), &(m_tris[i].m_norms[0][1]), &(m_tris[i].m_norms[0][2]),
      &(m_tris[i].m_matIdx[0]));

    fscanf( inFile, "v1 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[1][0]), &(m_tris[i].m_verts[1][1]), &(m_tris[i].m_verts[1][2]),
      &(m_tris[i].m_norms[1][0]), &(m_tris[i].m_norms[1][1]), &(m_tris[i].m_norms[1][2]),
      &(m_tris[i].m_matIdx[1]));

    fscanf( inFile, "v2 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[2][0]), &(m_tris[i].m_verts[2][1]), &(m_tris[i].m_verts[2][2]),
      &(m_tris[i].m_norms[2][0]), &(m_tris[i].m_norms[2][1]), &(m_tris[i].m_norms[2][2]),
      &(m_tris[i].m_matIdx[2]));

    fscanf( inFile, "face normal %f %f %f\n", &(m_tris[i].m_faceNorm[0]), &(m_tris[i].m_faceNorm[1]),
      &(m_tris[i].m_faceNorm[2]));
  }

  // Calculate bounding box
  GLfloat xmin = FLT_MAX;
  GLfloat ymin = FLT_MAX;
  GLfloat zmin = FLT_MAX;
  GLfloat xmax = -(FLT_MAX-1.0f);
  GLfloat ymax = -(FLT_MAX-1.0f);
  GLfloat zmax = -(FLT_MAX-1.0f);
  for ( GLuint i = 0; i < m_tris.size(); i++ ) {
    for ( GLuint j = 0; j < 3; j++ ) {
      xmin = fun::min( m_tris[i].m_verts[j][0], xmin );
      ymin = fun::min( m_tris[i].m_verts[j][1], ymin );
      zmin = fun::min( m_tris[i].m_verts[j][2], zmin );
      xmax = fun::max( m_tris[i].m_verts[j][0], xmax );
      ymax = fun::max( m_tris[i].m_verts[j][1], ymax );
      zmax = fun::max( m_tris[i].m_verts[j][2], zmax );
    }    
  }    
  m_box.m_min[0] = xmin;
  m_box.m_min[1] = ymin;
  m_box.m_min[2] = zmin;
  m_box.m_max[0] = xmax;
  m_box.m_max[1] = ymax;
  m_box.m_max[2] = zmax;

  // Calculate geometric center of model
  m_center[0] = (m_box.m_min[0] + m_box.m_max[0]) / 2.0f;
  m_center[1] = (m_box.m_min[1] + m_box.m_max[1]) / 2.0f;
  m_center[2] = (m_box.m_min[2] + m_box.m_max[2]) / 2.0f;

  fclose( inFile );
}

void Model::print() {
  printf( "# tris = %u\n", m_tris.size() );
  printf( "# mats = %u\n", m_mats.size() );
  printf( "box = (%f %f %f) (%f %f %f)\n", m_box.m_min[0], m_box.m_min[1], m_box.m_min[2], m_box.m_max[0], m_box.m_max[1], m_box.m_max[2] );
  printf( "center = (%f %f %f)\n", m_center[0], m_center[1], m_center[2] );
  printf( "mats\n" );
  for ( GLuint i = 0; i < m_mats.size(); i++ ) {
    printf( "  %i: amb = (%f %f %f)\n", i, m_mats[i].m_ambient[0], m_mats[i].m_ambient[1], m_mats[i].m_ambient[2] );
    printf( "  %i: dif = (%f %f %f)\n", i, m_mats[i].m_diffuse[0], m_mats[i].m_diffuse[1], m_mats[i].m_diffuse[2] );
    printf( "  %i: spec = (%f %f %f)\n", i, m_mats[i].m_specular[0], m_mats[i].m_specular[1], m_mats[i].m_specular[2] );
    printf( "  %i: %f\n", i, m_mats[i].m_shininess );
  }

  printf( "tris\n" );
  for ( GLuint i = 0; i < m_tris.size(); i++ ) {
    printf( "  t%u:\n", i );
    for ( GLuint j = 0; j < arraySize(m_tris[i].m_verts); j++ ) {
      printf( "    v%u %f %f %f | %f %f %f %d\n", j, 
        m_tris[i].m_verts[j][0], m_tris[i].m_verts[j][1], m_tris[i].m_verts[j][2],
        m_tris[i].m_norms[j][0], m_tris[i].m_norms[j][1], m_tris[i].m_norms[j][2],
        m_tris[i].m_matIdx[j] );
    }
  }
}
