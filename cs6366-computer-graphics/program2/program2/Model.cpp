#include "Model.h"

using namespace std;

Model::Model() { }

Model::~Model() {
  delete [] m_ambient;
  delete [] m_diffuse;
  delete [] m_specular;
  delete [] m_shine;
  delete [] m_box;
  delete [] m_center;
}

void Model::read( const string& path ) {
  // General index for looping.
  GLuint i, numTriangles, colorIndex[3];
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

  fscanf( inFile, "# triangles = %d\n", &numTriangles );
  fscanf( inFile, "Material count = %d\n", &m_materialCount );
  
  // Allocate heap memory for arrays
  m_ambient = new GLfloat[m_materialCount][3];
  m_diffuse = new GLfloat[m_materialCount][3];
  m_specular = new GLfloat[m_materialCount][3];
  m_shine = new GLfloat[m_materialCount];
  m_box = new GLfloat[6];
  m_center = new GLfloat[3];

  // Read materials
  for ( i = 0; i < m_materialCount; i++ ) {
    fscanf( inFile, "ambient color %f %f %f\n", &(m_ambient[i][0]), &(m_ambient[i][1]), &(m_ambient[i][2]) );
    fscanf( inFile, "diffuse color %f %f %f\n", &(m_diffuse[i][0]), &(m_diffuse[i][1]), &(m_diffuse[i][2]) );
    fscanf( inFile, "specular color %f %f %f\n", &(m_specular[i][0]), &(m_specular[i][1]), &(m_specular[i][2]) );
    fscanf( inFile, "material shine %f\n", &(m_shine[i]) );
  }

  fscanf( inFile, "%c", &fileIterator );
  while ( fileIterator != '\n' ) {
    fscanf( inFile, "%c", &fileIterator );
  }

  // Allocate and populate Triangles.
  m_tris.resize( numTriangles );
  for ( i = 0; i < numTriangles; i++ ) {
    fscanf( inFile, "v0 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[0][0]), &(m_tris[i].m_verts[0][1]), &(m_tris[i].m_verts[0][2]),
      &(m_tris[i].m_norms[0][0]), &(m_tris[i].m_norms[0][1]), &(m_tris[i].m_norms[0][2]),
      &(colorIndex[0]));

    fscanf( inFile, "v1 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[1][0]), &(m_tris[i].m_verts[1][1]), &(m_tris[i].m_verts[1][2]),
      &(m_tris[i].m_norms[1][0]), &(m_tris[i].m_norms[1][1]), &(m_tris[i].m_norms[1][2]),
      &(colorIndex[1]));

    fscanf( inFile, "v2 %f %f %f %f %f %f %d\n",
      &(m_tris[i].m_verts[2][0]), &(m_tris[i].m_verts[2][1]), &(m_tris[i].m_verts[2][2]),
      &(m_tris[i].m_norms[2][0]), &(m_tris[i].m_norms[2][1]), &(m_tris[i].m_norms[2][2]),
      &(colorIndex[2]));

    fscanf( inFile, "face normal %f %f %f\n", &(m_tris[i].m_faceNorm[0]), &(m_tris[i].m_faceNorm[1]),
      &(m_tris[i].m_faceNorm[2]));

    // I don't know what this does.
    m_tris[i].m_colorIndex[0] = (unsigned char)(int)(255*(m_diffuse[colorIndex[0]][0]));
    m_tris[i].m_colorIndex[1] = (unsigned char)(int)(255*(m_diffuse[colorIndex[0]][1]));
    m_tris[i].m_colorIndex[2] = (unsigned char)(int)(255*(m_diffuse[colorIndex[0]][2]));
  }

  // Calculate bounding box
  GLfloat xmin = FLT_MAX;
  GLfloat ymin = FLT_MAX;
  GLfloat zmin = FLT_MAX;
  GLfloat xmax = -(FLT_MAX-1.0f);
  GLfloat ymax = -(FLT_MAX-1.0f);
  GLfloat zmax = -(FLT_MAX-1.0f);
  GLfloat x;
  GLfloat y;
  GLfloat z;
  for ( GLuint i = 0; i < m_tris.size(); i++ ) {
    for ( GLuint j = 0; j < 3; j++ ) {
      x = m_tris[i].m_verts[j][0];
      y = m_tris[i].m_verts[j][1];
      z = m_tris[i].m_verts[j][2];
      if ( x < xmin ) { xmin = x; }
      if ( x > xmax ) { xmax = x; }
      if ( y < ymin ) { ymin = y; }
      if ( y > ymax ) { ymax = y; }
      if ( z < zmin ) { zmin = z; }
      if ( z > zmax ) { zmax = z; }
    }    
  }    
  m_box[0] = xmin;
  m_box[1] = ymin;
  m_box[2] = zmin;
  m_box[3] = xmax;
  m_box[4] = ymax;
  m_box[5] = zmax;

  // Calculate geometric center of model
  m_center[0] = (m_box[0] + m_box[3]) / 2.0f;
  m_center[1] = (m_box[1] + m_box[4]) / 2.0f;
  m_center[2] = (m_box[2] + m_box[5]) / 2.0f;

  fclose( inFile );
}

void Model::print() {
  printf( "Triangles = %u\n", m_tris.size() );
  printf( "Materials = %u\n", m_materialCount );
  printf( "Bounding box = (%f %f %f) (%f %f %f)\n", 
    m_box[0], m_box[1], m_box[2], m_box[3], m_box[4], m_box[5] );
  printf( "Center = %f %f %f\n", m_center[0], m_center[1], m_center[2] );
  printf( "Ambient colors:\n" );
  for ( GLuint i = 0; i < m_materialCount; i++ ) {
    printf( "  %i: %f %f %f\n", i, m_ambient[i][0], m_ambient[i][1], m_ambient[i][2] );
  }
  printf( "Diffuse colors:\n" );
  for ( GLuint i = 0; i < m_materialCount; i++ ) {
    printf( "  %i: %f %f %f\n", i, m_diffuse[i][0], m_diffuse[i][1], m_diffuse[i][2] );
  }
  printf( "Specular colors:\n" );
  for ( GLuint i = 0; i < m_materialCount; i++ ) {
    printf( "  %i: %f %f %f\n", i, m_specular[i][0], m_specular[i][1], m_specular[i][2] );
  }
  printf( "Shines:\n" );
  for ( GLuint i = 0; i < m_materialCount; i++ ) {
    printf( "  %i: %f\n", i, m_shine[i] );
  }
  for ( GLuint i = 0; i < m_tris.size(); i++ ) {
    printf( "t%u:\n", i );
    for ( GLuint j = 0; j < arraySize(m_tris[i].m_verts); j++ ) {
      printf( "  v%u %f %f %f | %f %f %f %d\n", j, 
        m_tris[i].m_verts[j][0], m_tris[i].m_verts[j][1], m_tris[i].m_verts[j][2],
        m_tris[i].m_norms[j][0], m_tris[i].m_norms[j][1], m_tris[i].m_norms[j][2],
        m_tris[i].m_colorIndex[j] );
    }
  }
}
