#include "GLCamera.h"

using namespace std;

GLCamera::GLCamera() { 
  init();
}

GLvoid GLCamera::init() {
  FOVY = 60.0;
  FOVX = 60.0;
  ASPECT_RATIO = 1.0;  
  NEAR_CLIPPING_PLANE = 1.0;
  FAR_CLIPPING_PLANE = 10e4;
  DEFAULT_TRANSLATE_STEP = 0.0;
  DEFAULT_ROTATE_ANGLE = 0.0;

  memset( m_translateDelta, 0, 3*sizeof(GLfloat) );
  memset( m_rotateDelta,    0, 3*sizeof(GLfloat) );
  memset( m_projMatr,       0, 16*sizeof(GLfloat) );
  memset( m_modelViewMatr,  0, 16*sizeof(GLfloat) );
  memset( m_tempMatr,       0, 16*sizeof(GLfloat) );
  memset( m_pos, 0, 4*sizeof(GLfloat) ); m_pos[3] = 1.0f;

  m_invertYLook = false;
  m_invertXLook = false;
  m_fovY = FOVY;
  m_fovX = FOVX;
  m_aspect = ASPECT_RATIO;
  m_zNear = NEAR_CLIPPING_PLANE;
  m_zFar = FAR_CLIPPING_PLANE;
  m_clipDelta = 0.05f;
}
  
GLCamera::~GLCamera() { }

GLvoid GLCamera::translate( GLfloat x, GLfloat y, GLfloat z ) {
  glMatrixMode( GL_MODELVIEW );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
  glLoadIdentity();
  glTranslatef( x, y, z );
  glMultMatrixf( m_modelViewMatr );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
}

GLvoid GLCamera::translate( const vec3f& v ) {
  translate( v[0], v[1], v[2] );
}

GLvoid GLCamera::translate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    translate( dir * m_translateDelta[0], 0.0, 0.0 );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    translate( 0.0, dir * m_translateDelta[1], 0.0 );
  }

  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    translate(  0.0, 0.0, dir * m_translateDelta[2] );
  }
}

GLvoid GLCamera::rotate( GLubyte axis, GLfloat theta ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    rotateU( theta );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    rotateV( theta );
  }
  
  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    rotateN( theta );
  }
}

GLvoid GLCamera::rotate( const vec3f& v, GLfloat theta ) {
  /* NOT IN USE ATM */
}

GLvoid GLCamera::rotate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    rotateU( dir * m_rotateDelta[0] );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    rotateV( dir * m_rotateDelta[1] );
  }
  
  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    rotateN( dir * m_rotateDelta[2] );
  }
}

GLvoid GLCamera::rotateU( GLfloat theta ) {  
  glMatrixMode( GL_MODELVIEW );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
  glLoadIdentity();
  glRotatef( theta, 1.0, 0.0, 0.0 );
  glMultMatrixf( m_modelViewMatr );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
}

GLvoid GLCamera::rotateV( GLfloat theta ) {
  glMatrixMode( GL_MODELVIEW );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
  glLoadIdentity();
  glRotatef( theta, 0.0, 1.0, 0.0 );
  glMultMatrixf( m_modelViewMatr );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
}

GLvoid GLCamera::rotateN( GLfloat theta ) {
  glMatrixMode( GL_MODELVIEW );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
  glLoadIdentity();
  glRotatef( theta, 0.0, 0.0, 1.0 );
  glMultMatrixf( m_modelViewMatr );
  glGetFloatv( GL_MODELVIEW_MATRIX, m_modelViewMatr );
}

GLvoid GLCamera::moveClippingPlane( GLubyte plane, GLint dir ) {
  if ( plane == 'f' ) {
    m_zFar *= (1.0f + ((GLfloat)dir * m_clipDelta));
    if ( m_zFar < m_zNear ) { m_zFar = m_zNear; }
  }
  if ( plane == 'n' ) {
    m_zNear *= (1.0f + ((GLfloat)dir * m_clipDelta));
    if ( m_zNear > m_zFar ) { m_zNear = m_zFar; }
  }

  updatePerspective( m_aspect ); 
}

GLvoid GLCamera::updatePerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar ) {
  m_aspect = aspect;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();  
  gluPerspective( fovy, m_aspect, zNear, zFar );
  glGetFloatv( GL_PROJECTION_MATRIX, m_projMatr );
}

GLvoid GLCamera::updatePerspective( GLfloat aspect) {
  updatePerspective( m_fovY, aspect, m_zNear, m_zFar );
}

GLvoid GLCamera::centerOn( const Model& m, GLint width, GLint height ) {
  // Update model view matrix to be equivalent to lookAt
  GLfloat modelHeight = (m.m_box[4] - m.m_box[1]);
  GLfloat modelWidth = (m.m_box[3] - m.m_box[0]);
  GLfloat ez;
  if ( modelWidth <= modelHeight ) {
    ez = m.m_center[2] + modelHeight/(GLfloat)tan((FOVY/2.0)*conv::DegToRad);
  }
  else {
    ez = m.m_center[2] + modelWidth/(GLfloat)tan((FOVY/2.0)*conv::DegToRad);
  } 

  // Update modelview matrix
  lookAt( m.m_center[0], m.m_center[1], ez,
          m.m_center[0], m.m_center[1], m.m_center[2],
          0,             1,             0 );

  // Update clipping planes to fit model
  m_zNear = NEAR_CLIPPING_PLANE;
  m_zFar = ez 
    + fun::dist(m.m_center[0], m.m_center[1], ez, 
                m.m_center[0], m.m_center[1], m.m_box[5] - m.m_box[2] ) + 10.0f;

  // Update perspective matrix
  updatePerspective( (GLfloat)width / (GLfloat)height );

  // Update deltas for proper translation/rotation control
  calcTranslateDelta( m );
  calcRotateDelta( m );
}

GLvoid GLCamera::lookAt( GLfloat ex, GLfloat ey, GLfloat ez,
                         GLfloat cx, GLfloat cy, GLfloat cz,
                         GLfloat ux, GLfloat uy, GLfloat uz ) {
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  gluLookAt( ex, ey, ez, cx, cy, cz, ux, uy, uz );   
  glGetFloatv( GL_MODELVIEW_MATRIX, m_projMatr );
}

GLvoid GLCamera::print() {
  cout << "GLCamera:" << endl;
  cout << "  fovy = " << m_fovY << endl;
  cout << "  aspect = " << m_aspect << endl;
  cout << "  near, far = " << m_zNear << " " << m_zFar << endl;
  cout << "  modelview = " << endl;
  cout << "    [" << m_modelViewMatr[0] << " " << m_modelViewMatr[4] << " " << m_modelViewMatr[8] << " " << m_modelViewMatr[12] << "]" << endl;
  cout << "    [" << m_modelViewMatr[1] << " " << m_modelViewMatr[5] << " " << m_modelViewMatr[9] << " " << m_modelViewMatr[13] << "]" << endl;
  cout << "    [" << m_modelViewMatr[2] << " " << m_modelViewMatr[6] << " " << m_modelViewMatr[10] << " " << m_modelViewMatr[14] << "]" << endl;
  cout << "    [" << m_modelViewMatr[3] << " " << m_modelViewMatr[7] << " " << m_modelViewMatr[11] << " " << m_modelViewMatr[15] << "]" << endl;

  cout << "  projection = " << endl;
  cout << "    [" << m_projMatr[0] << " " << m_projMatr[4] << " " << m_projMatr[8] << " " << m_projMatr[12] << "]" << endl;
  cout << "    [" << m_projMatr[1] << " " << m_projMatr[5] << " " << m_projMatr[9] << " " << m_projMatr[13] << "]" << endl;
  cout << "    [" << m_projMatr[2] << " " << m_projMatr[6] << " " << m_projMatr[10] << " " << m_projMatr[14] << "]" << endl;
  cout << "    [" << m_projMatr[3] << " " << m_projMatr[7] << " " << m_projMatr[11] << " " << m_projMatr[15] << "]" << endl;
}
