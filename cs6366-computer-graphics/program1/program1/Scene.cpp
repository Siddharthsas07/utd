#include "GL/freeglut.h"
#include "Scene.h"
#include "Camera.h"
#include "Window.h"
#include <iostream>

using namespace std;

Scene::Scene() {
  COLOR_CHANGE = 0.05;

  m_numModels = 0; 
  m_models = new Model[m_numModels];
  m_modChoice = 0;
  memset(m_rColor, 1, arraySize(m_rColor));
  m_rColorDelta = COLOR_CHANGE;
  m_cam = new Camera();
  m_ccw = GL_TRUE;
  m_backfaceCulling = GL_TRUE;
}

Scene::Scene( GLuint numModels ) {
  COLOR_CHANGE = 0.05;

  m_numModels = numModels; 
  m_models = new Model[m_numModels];
  m_modChoice = 0;
  memset(m_rColor, 1, arraySize(m_rColor));
  m_rColorDelta = COLOR_CHANGE;
  m_cam = new Camera();
  m_ccw = GL_TRUE;
  m_backfaceCulling = GL_TRUE;
}

Scene::~Scene() {
  delete [] m_models;
  delete m_cam;
}

GLvoid Scene::print() {
  cout << "Window = " << windowWidth << "x" << windowHeight << endl;
  cout << "Subwindow = " << subwindowWidth << "x" << subwindowHeight << endl;
  cout << "Model choice = " << m_modChoice << endl;
  cout << "Draw color = (" << m_rColor[0] << " " << m_rColor[1] << " " 
    << m_rColor[2] << " " << m_rColor[3] << ")" << endl;
  cout << "Render mode = " << (GLint)m_rMode << endl;
  cout << "CCW drawing = " << m_ccw << endl;
  cout << "Backface culling = " << m_backfaceCulling << endl;

  m_cam->print();
}

GLvoid Scene::changeColor( GLubyte value, GLbyte dir ) {
  if ( value == 'r' ) { m_rColor[0] += ((GLdouble)dir * m_rColorDelta); }
  if ( value == 'g' ) { m_rColor[1] += ((GLdouble)dir * m_rColorDelta); }
  if ( value == 'b' ) { m_rColor[2] += ((GLdouble)dir * m_rColorDelta); }
  if ( value == 'a' ) { m_rColor[3] += ((GLdouble)dir * m_rColorDelta); }
}

GLvoid Scene::drawCurrentModel() {
  glFrontFace( m_ccw ? GL_CCW : GL_CW );

  if ( m_backfaceCulling == GL_TRUE ) { glEnable( GL_CULL_FACE ); }
  else { glDisable( GL_CULL_FACE ); }

  if ( m_rMode == R_POINT ) {
    glPolygonMode( GL_FRONT_AND_BACK, GL_POINTS );
    glBegin( GL_POINTS );
  }
  
  else if ( m_rMode == R_WIREFRAME ) { 
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin( GL_TRIANGLES );
  }

  else if ( m_rMode == R_SOLID ) { 
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin( GL_TRIANGLES );
  }

  // Set model color 
  glColor4d( m_rColor[0], m_rColor[1], m_rColor[2], m_rColor[3] );

  for ( GLuint i = 0; i < m_models[m_modChoice].m_tris.size(); i++ ) {
    for ( GLuint j = 0; j < 3; j++ ) {
      // TODO use arraylist/displaylist somewhere 
      glVertex3fv( m_models[m_modChoice].m_tris[i].m_verts[j] );
    }    
  }

  glEnd();
}

GLvoid Scene::centerCameraOnCurrentModel() {
  m_cam->centerOn( &(m_models[m_modChoice]) );
}

GLvoid Scene::calcPerspectiveForCurrentModel()  {
  m_cam->calcPerspective( subwindowWidth, subwindowHeight /*&(m_models[m_modChoice])*/ );
}

GLvoid Scene::calcTranslateStepsForCurrentModel() {
  m_cam->calcTranslateSteps( &(m_models[m_modChoice]) );
}

GLvoid Scene::setColor( col4d* c ) {
  setColor( *c[0], *c[1], *c[2], *c[2] );
}

GLvoid Scene::setColor( col3d* c ) {
  setColor( *c[0], *c[1], *c[2], 1.0 );
}

GLvoid Scene::setColor( GLdouble r, GLdouble g, GLdouble b ) {
  setColor( r, g, b, 1.0 );
}

GLvoid Scene::setColor( GLdouble r, GLdouble g, GLdouble b, GLdouble a ) {
  m_rColor[0] = r;  m_rColor[1] = g;  m_rColor[2] = b;  m_rColor[3] = a;
}