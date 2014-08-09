#include "glTypes.h"
#include "useful_things.h"
#include "GL/freeglut.h"
#include "Model.h"
#include "Camera.h"
#include "Window.h"
#include <cmath>
#include <iostream>

using namespace std;

extern GLuint windowWidth;
extern GLuint GLwindowHeight;
extern GLuint subwindowWidth;
extern GLuint subwindowHeight;
extern GLubyte borderSize;

GLvoid Camera::initializeMembers() {
  FOVY = 60.0;
  FOVX = 60.0;
  ASPECT_RATIO = 1.0;
  NEAR_CLIPPING_PLANE = 1.0;
  FAR_CLIPPING_PLANE = 10e4;
  DEFAULT_TRANSLATE_STEP = 0.0;
  DEFAULT_ROTATE_ANGLE = 0.0;

  m_perspective[0] = FOVY;
  m_perspective[1] = ASPECT_RATIO;
  m_perspective[2] = NEAR_CLIPPING_PLANE;
  m_perspective[3] = FAR_CLIPPING_PLANE;

  memset(m_lookAt, 0, arraySize(m_lookAt));
  m_lookAt[7] = 1.0;
  memset(m_userTranslation, 0, arraySize(m_userTranslation));
  memset(m_userRotation, 0, arraySize(m_userRotation));
  memset(m_translateStep, (GLint)DEFAULT_TRANSLATE_STEP, arraySize(m_translateStep));
  memset(m_rotateStep, (GLint)DEFAULT_ROTATE_ANGLE, arraySize(m_rotateStep));

  m_invertYLook = false;
  m_invertXLook = false;
  m_fovY = FOVY;
  m_fovX = FOVX;
  m_aspectRatio = ASPECT_RATIO;
  m_nearClippingPlane = NEAR_CLIPPING_PLANE;
  m_farClippingPlane = FAR_CLIPPING_PLANE;
  m_clippingPlaneDelta = 0.05;
}

Camera::Camera() { 
  initializeMembers();
}
  
Camera::~Camera() { }

GLvoid Camera::translate( GLfloat x, GLfloat y, GLfloat z ) {
  translate( (GLdouble)x, (GLdouble)y, (GLdouble)z );
}

GLvoid Camera::translate( GLdouble x, GLdouble y, GLdouble z ) {
  m_userTranslation[0] = x;
  m_userTranslation[1] = y;
  m_userTranslation[2] = z;
  glMatrixMode( GL_MODELVIEW );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  glLoadIdentity();
  glTranslated( x, y, z );
  glMultMatrixd( m_modelviewMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  print();
}

GLvoid Camera::translate( vec3f* v ) {
  translate( (GLdouble) *(v[0]), (GLdouble) *(v[1]), (GLdouble) * (v[2]) );
}

GLvoid Camera::translate( vec3d* v ) {
  translate( *(v[0]), *(v[1]), *(v[2]) );
}

GLvoid Camera::translate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    translate( dir * m_translateStep[0], 0.0, 0.0 );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    translate( 0.0, dir * m_translateStep[1], 0.0 );
  }

  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    translate( 0.0, 0.0, dir * m_translateStep[2] );
  }
}

GLvoid Camera::rotate( GLubyte axis, GLfloat theta ) {
  rotate( axis, (GLdouble)theta );
}

GLvoid Camera::rotate( GLubyte axis, GLdouble theta ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    m_userRotation[0] = 1.0;
    m_userRotation[1] = 0.0;
    m_userRotation[2] = 0.0;
    if ( m_invertYLook == GL_TRUE ) { theta *= -1.0; }
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    m_userRotation[0] = 0.0;
    m_userRotation[1] = 1.0;
    m_userRotation[2] = 0.0;
    if ( m_invertXLook == GL_TRUE ) { theta *= -1.0; }
  }

  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    m_userRotation[0] = 0.0;
    m_userRotation[1] = 0.0;
    m_userRotation[2] = 1.0;
  }

  glMatrixMode( GL_MODELVIEW );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  glLoadIdentity();
  glRotated( theta, m_userRotation[0], m_userRotation[1], m_userRotation[2] );
  glMultMatrixd( m_modelviewMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  print();
}

GLvoid Camera::rotate( vec3f* v, GLfloat theta ) {
  m_userRotation[0] = (GLdouble) *(v[0]);
  m_userRotation[1] = (GLdouble) *(v[1]);
  m_userRotation[2] = (GLdouble) *(v[2]);

  glMatrixMode( GL_MODELVIEW );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  glLoadIdentity();
  glRotated( theta, m_userRotation[0], m_userRotation[1], m_userRotation[2] );
  glMultMatrixd( m_modelviewMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
}

GLvoid Camera::rotate( vec3d* v, GLdouble theta ) {
  m_userRotation[0] = *(v[0]);
  m_userRotation[1] = *(v[1]);
  m_userRotation[2] = *(v[2]);

  glMatrixMode( GL_MODELVIEW );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
  glLoadIdentity();
  glRotated( theta, m_userRotation[0], m_userRotation[1], m_userRotation[2] );
  glMultMatrixd( m_modelviewMatrix );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );
}

GLvoid Camera::rotate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    rotate( axis, dir * m_rotateStep[0] );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    rotate( axis, dir * m_rotateStep[1] );
  }

  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    rotate( axis, dir * m_rotateStep[2] );
  }
}

GLvoid Camera::moveClippingPlane( GLubyte plane, GLint dir ) {
  if ( plane == 'f' ) {
    cout << "updating a clipping plane from " << m_farClippingPlane << " to " << m_farClippingPlane * (1.0 + ((GLdouble)dir * m_clippingPlaneDelta)) << endl;
    m_farClippingPlane *= (1.0 + ((GLdouble)dir * m_clippingPlaneDelta));
  }
  if ( plane == 'n' ) {
    cout << "updating a clipping plane from " << m_nearClippingPlane << " to " << m_farClippingPlane * (1.0 + ((GLdouble)dir * m_nearClippingPlane)) << endl;
    m_nearClippingPlane *= (1.0 + ((GLdouble)dir * m_clippingPlaneDelta));
  }

  calcPerspective( subwindowWidth, subwindowHeight );
  execPerspectiveChange();
}

GLvoid Camera::centerOn( Model* m ) {
  // Clear the translation and rotation.
  m_userTranslation[0] = 0;
  m_userTranslation[1] = 0;
  m_userTranslation[2] = 0;
  m_userRotation[0] = 0;
  m_userRotation[1] = 0;
  m_userRotation[2] = 0;

  // Calculate camera location for model.
  m_lookAt[0] = m->m_center[0];
  m_lookAt[1] = m->m_center[1];
  GLdouble modelHeight = (m->m_box[4] - m->m_box[1]);
  GLdouble modelWidth = (m->m_box[3] - m->m_box[0]);
  GLdouble ez;
  if ( modelWidth <= modelHeight ) {
    ez = m_lookAt[2] = m->m_center[2] + modelHeight/tan((FOVY/2.0)*conv::DegToRad);
  }
  else {
    ez = m_lookAt[2] = m->m_center[2] + modelWidth/tan((FOVY/2.0)*conv::DegToRad);
  }  
  m_lookAt[3] = m->m_center[0];
  m_lookAt[4] = m->m_center[1];
  m_lookAt[5] = m->m_center[2];
  m_lookAt[6] = 0.0;
  m_lookAt[7] = 1.0;
  m_lookAt[8] = 0.0;

  // Execute camera relocation for model.
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  gluLookAt(
    m_lookAt[0], m_lookAt[1], m_lookAt[2],
    m_lookAt[3], m_lookAt[4], m_lookAt[5],
    m_lookAt[6], m_lookAt[7], m_lookAt[8] );
  glGetDoublev( GL_MODELVIEW_MATRIX, m_modelviewMatrix );

  // Calculate the new clipping planes and perspective
  // from where the camera is located
  m_nearClippingPlane = NEAR_CLIPPING_PLANE;
  m_farClippingPlane = m_lookAt[2] 
    + fun::dist(m_lookAt[0], m_lookAt[1], m_lookAt[2], 
             (GLdouble)m->m_center[0], (GLdouble)m->m_center[1], (GLdouble)(m->m_box[5] - m->m_box[2]) )
    + 10.0;
  calcPerspective( subwindowWidth, subwindowHeight ); 
  execPerspectiveChange();

  // Recalulate the step width for translations, as the model could have changed. 
  calcTranslateSteps( m );

  // Recalculate the angle for rotations.
  calcRotateAngles( m );

  print();
}

GLvoid Camera::calcPerspective( GLint width, GLint height ) {
  // Recalculate the perspective values.
  m_perspective[0] = FOVY;
  m_aspectRatio = (GLdouble)width / (GLdouble)height;
  m_perspective[1] = m_aspectRatio;
  m_perspective[2] = m_nearClippingPlane;
  m_perspective[3] = m_farClippingPlane;
}

GLvoid Camera::calcTranslateSteps( Model* m ) {
  // Move 5% of model width/etc for each translation
  m_translateStep[0] = (m->m_box[3] - m->m_box[0]) * .05;
  m_translateStep[1] = (m->m_box[4] - m->m_box[1]) * .05;
  m_translateStep[2] = (m->m_box[5] - m->m_box[2]) * .05;
}

GLvoid Camera::calcRotateAngles( Model* m ) {
  // 50 rotations to complete a circle
  m_rotateStep[0] = 360.0 / 50.0;
  m_rotateStep[1] = 360.0 / 50.0;
  m_rotateStep[2] = 360.0 / 50.0;
}

GLvoid Camera::execPerspectiveChange() {
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();  
  gluPerspective( m_perspective[0], m_perspective[1], m_perspective[2], m_perspective[3] );
  glGetDoublev( GL_PROJECTION_MATRIX, m_projectionMatrix );
}

GLvoid Camera::print() {
  cout << "lookAt = " << endl;
  cout << "  [" << m_lookAt[0] << " " << m_lookAt[1] << " " << m_lookAt[2] << "]" << endl;
  cout << "  [" << m_lookAt[3] << " " << m_lookAt[4] << " " << m_lookAt[5] << "]" << endl;
  cout << "  [" << m_lookAt[6] << " " << m_lookAt[7] << " " << m_lookAt[8] << "]" << endl;
  cout << "perspective = " << endl;
  cout << "  [" << m_perspective[0] << " " << m_perspective[1] 
    << " " << m_perspective[2] << " " << m_perspective[3] << "]" << endl;
  cout << "modelview = " << endl;
  cout << "  [" << m_modelviewMatrix[0] << " " << m_modelviewMatrix[4] << " " << m_modelviewMatrix[8] << " " << m_modelviewMatrix[12] << "]" << endl;
  cout << "  [" << m_modelviewMatrix[1] << " " << m_modelviewMatrix[5] << " " << m_modelviewMatrix[9] << " " << m_modelviewMatrix[13] << "]" << endl;
  cout << "  [" << m_modelviewMatrix[2] << " " << m_modelviewMatrix[6] << " " << m_modelviewMatrix[10] << " " << m_modelviewMatrix[14] << "]" << endl;
  cout << "  [" << m_modelviewMatrix[3] << " " << m_modelviewMatrix[7] << " " << m_modelviewMatrix[11] << " " << m_modelviewMatrix[15] << "]" << endl;
  cout << "projection = " << endl;
  cout << "  [" << m_projectionMatrix[0] << " " << m_projectionMatrix[4] << " " << m_projectionMatrix[8] << " " << m_projectionMatrix[12] << "]" << endl;
  cout << "  [" << m_projectionMatrix[1] << " " << m_projectionMatrix[5] << " " << m_projectionMatrix[9] << " " << m_projectionMatrix[13] << "]" << endl;
  cout << "  [" << m_projectionMatrix[2] << " " << m_projectionMatrix[6] << " " << m_projectionMatrix[10] << " " << m_projectionMatrix[14] << "]" << endl;
  cout << "  [" << m_projectionMatrix[3] << " " << m_projectionMatrix[7] << " " << m_projectionMatrix[11] << " " << m_projectionMatrix[15] << "]" << endl;

}