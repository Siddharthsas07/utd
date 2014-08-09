#include "ShaderCamera.h"

using namespace std;

ShaderCamera::ShaderCamera() { 
  init();
}

GLvoid ShaderCamera::init() {
  FOVY = 60.0f;
  FOVX = 60.0f;
  ASPECT_RATIO = 1.0f;  
  NEAR_CLIPPING_PLANE = 1.0f;
  FAR_CLIPPING_PLANE = 10e4f;
  DEFAULT_TRANSLATE_STEP = .05f;
  DEFAULT_ROTATE_ANGLE = 360.0f/50.0f;

  memset( m_translateDelta, 0, sizeof(m_translateDelta) );
  memset( m_rotateDelta,    0, sizeof(m_rotateDelta) );
  memset( m_projMatr,       0, sizeof(m_projMatr) );
  memset( m_modelViewMatr,  0, sizeof(m_modelViewMatr) );
  memset( m_tempMatr,       0, sizeof(m_tempMatr) );
  memset( m_pos,            0, sizeof(m_pos) );

  m_invertYLook = false;
  m_invertXLook = false;
  m_fovY = FOVY;
  m_fovX = FOVX;
  m_aspect = ASPECT_RATIO;
  m_zNear = NEAR_CLIPPING_PLANE;
  m_zFar = FAR_CLIPPING_PLANE;
  m_clipDelta = DEFAULT_TRANSLATE_STEP;
}
  
ShaderCamera::~ShaderCamera() { }

GLvoid ShaderCamera::translate( GLfloat x, GLfloat y, GLfloat z ) {
  translate( m_modelViewMatr, x, y, z );
}

GLvoid ShaderCamera::translate( const vec3& v ) {
  translate( m_modelViewMatr, v[0], v[1], v[2] );
}

GLvoid ShaderCamera::translate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    translate( m_modelViewMatr, dir * m_translateDelta[0], 0.0, 0.0 );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    translate( m_modelViewMatr, 0.0, dir * m_translateDelta[1], 0.0 );
  }

  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    translate( m_modelViewMatr, 0.0, 0.0, dir * m_translateDelta[2] );
  }
}

GLvoid ShaderCamera::translate( mat4& m, GLfloat x, GLfloat y, GLfloat z ) {
  // Transpose of a translation matrix, because column major format
  mat4 T_t = { 1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1, 0,
               x, y, z, 1 };
  // (T * M) * v -> v * (M * T) because column major format
  multMatrix( m, m, T_t );
}


GLvoid ShaderCamera::rotate( GLubyte axis, GLfloat theta ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    rotateU( m_modelViewMatr, theta );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    rotateV( m_modelViewMatr, theta );
  }
  
  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    rotateN( m_modelViewMatr, theta );
  }
}

GLvoid ShaderCamera::rotate( const vec3& v, GLfloat theta ) {
  /* NOT IN USE ATM */
}

GLvoid ShaderCamera::rotate( GLubyte axis, GLint dir ) {
  if ( axis == 'u' || axis == 'U' || axis == 'x' || axis == 'X' ) {
    rotateU( m_modelViewMatr, dir * m_rotateDelta[0] );
  }

  else if ( axis == 'v' || axis == 'V' || axis == 'y' || axis == 'Y' ) {
    rotateV( m_modelViewMatr, dir * m_rotateDelta[1] );
  }
  
  else if ( axis == 'n' || axis == 'N' || axis == 'z' || axis == 'Z' ) {
    rotateN( m_modelViewMatr, dir * m_rotateDelta[2] );
  }
}

GLvoid ShaderCamera::rotateU( mat4& m, GLfloat theta ) {
  theta *= (GLfloat)conv::DegToRad;
  // Transpose of a rotation matrix because column major format
  mat4 R_t = { 1, 0,           0,          0,
               0, cos(theta),  sin(theta), 0,
               0, -sin(theta), cos(theta), 0,
               0,  0,          0,          1 };

  // (R * M) * v -> v * (M * R) because column major format
  multMatrix( m, m, R_t );
}

GLvoid ShaderCamera::rotateV( mat4& m, GLfloat theta ) {
  theta *= (GLfloat)conv::DegToRad;
  // Transpose of a rotation matrix because column major format
  mat4 R_t = { cos(theta), 0, -sin(theta), 0,
               0,          1, 0,           0,
               sin(theta), 0, cos(theta),  0,
               0,          0, 0,           1 };

  // (R * M) * v -> v * (M * R) because column major format
  multMatrix( m, m, R_t );
}

GLvoid ShaderCamera::rotateN( mat4& m, GLfloat theta ) {
  theta *= (GLfloat)conv::DegToRad;

  // Transpose of a rotation matrix because column major format
  mat4 R_t = { cos(theta),  sin(theta), 0, 0,
               -sin(theta), cos(theta), 0, 0,
               0,           0,          1, 0,
               0,           0,          0, 1 };

  // (R * M) * v -> v * (M * R) because column major format
  multMatrix( m, m, R_t );
}

GLvoid ShaderCamera::moveClippingPlane( GLubyte plane, GLint dir ) {
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

/* http://nehe.gamedev.net/article/replacement_for_gluperspective/21002/ */
GLvoid ShaderCamera::updatePerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar ) {
  m_aspect = aspect;
  GLfloat ymax = zNear * (GLfloat)tan(fovy * conv::DegToRad / 2.0f);
  GLfloat xmax = ymax * m_aspect;
  setFrustum( -xmax, xmax, -ymax, ymax, zNear, zFar );
}

GLvoid ShaderCamera::updatePerspective( GLfloat aspect) {
  updatePerspective( m_fovY, aspect, m_zNear, m_zFar );
}

GLvoid ShaderCamera::setFrustum( GLfloat l, GLfloat r, GLfloat b, 
                                 GLfloat t, GLfloat n, GLfloat f ) {
  m_projMatr[0] = 2*n/(r-l);  m_projMatr[1] = 0;          m_projMatr[2] = 0;            m_projMatr[3] = 0;
  m_projMatr[4] = 0;          m_projMatr[5] = 2*n/(t-b) ; m_projMatr[6] = 0;            m_projMatr[7] = 0;
  m_projMatr[8] = (r+l)/(r-l);m_projMatr[9] = (t+b)/(t-b);m_projMatr[10] = -(f+n)/(f-n);m_projMatr[11] = -1;
  m_projMatr[12] = 0;         m_projMatr[13] = 0;         m_projMatr[14] = -2*f*n/(f-n);m_projMatr[15] = 0; 
}

GLvoid ShaderCamera::centerOn( const Model& m, GLint width, GLint height ) {
  // Update model view matrix to be equivalent to lookAt
  GLfloat modelHeight = (m.m_box.m_max[1] - m.m_box.m_min[1]);
  GLfloat modelWidth = (m.m_box.m_max[0] - m.m_box.m_min[0]);
  GLfloat ez;
  if ( modelWidth <= modelHeight ) {
    ez = m.m_center[2] + modelHeight/(GLfloat)tan((FOVY/2.0)*conv::DegToRad);
  }
  else {
    ez = m.m_center[2] + modelWidth/(GLfloat)tan((FOVY/2.0)*conv::DegToRad);
  } 

  identity( m_modelViewMatr );
  lookAt( m.m_center[0], m.m_center[1], ez,
          m.m_center[0], m.m_center[1], m.m_center[2],
          0,             1,             0 );

  // Update clipping planes to fit model
  m_zNear = NEAR_CLIPPING_PLANE;
  m_zFar = ez 
    + fun::dist(m.m_center[0], m.m_center[1], ez, 
                m.m_center[0], m.m_center[1], m.m_box.m_max[2] - m.m_box.m_min[2] ) + 10.0f;

  // Update perspective matrix
  updatePerspective( (GLfloat)width / (GLfloat)height );

  // Update deltas for proper translation/rotation control
  calcTranslateDelta( m );
  calcRotateDelta( m );
}

/* http://www.opengl.org/wiki/GluLookAt_code */
GLvoid ShaderCamera::lookAt( GLfloat ex, GLfloat ey, GLfloat ez,
                             GLfloat cx, GLfloat cy, GLfloat cz,
                             GLfloat ux, GLfloat uy, GLfloat uz ) {
  //m_pos[0] = ex; m_pos[1] = ey; m_pos[2] = ez; m_pos[3] = 1.0f; // Necessary?
  vec3 forward = {cx-ex, cy-ey, cz-ez};
  vec3 side = {0, 0, 0};
  vec3 up = {ux, uy, uz};
  normalize( forward );

  // side = forward x up
  cross( side, forward, up ); 
  normalize( side );

  // up = forward x side
  cross( up, forward, side );

  mat4 temp = { side[0], up[0], -forward[0], 0,
                side[1], up[1], -forward[1], 0,
                side[2], up[2], -forward[2], 0,
                0,       0,     0,           1 };

  // m_modelViewMatr = temp translated by -ex, -ey, -ez.
  multMatrix( temp, m_modelViewMatr, temp );
  translate( temp, -ex, -ey, -ez );
  memcpy( &m_modelViewMatr, &temp, sizeof(m_modelViewMatr) );
}

GLvoid ShaderCamera::normalize( vec3& v ) {
  GLfloat length = sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
  if ( length != 0 ) {
    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
  }
  else {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
  }
}

GLvoid ShaderCamera::identity( mat4& m ) {
  memset( m, 0, sizeof(m) );
  m[0] = 1;
  m[5] = 1;
  m[10] = 1;
  m[15] = 1;
}

GLvoid ShaderCamera::transpose( mat4& m ) {
  mat4 temp;
  temp[0] = m[0];  temp[4] = m[1];  temp[8] = m[2];   temp[12] = m[3];
  temp[1] = m[4];  temp[5] = m[5];  temp[9] = m[6];   temp[13] = m[7];
  temp[2] = m[8];  temp[6] = m[9];  temp[10] = m[10]; temp[14] = m[11];
  temp[3] = m[12]; temp[7] = m[13]; temp[11] = m[14]; temp[15] = m[15];
  memcpy( m, &temp, sizeof(m) );
}

GLvoid ShaderCamera::multMatrix( mat4& res, const mat4& lhs, const mat4& rhs ) {
  mat4 temp;
  temp[0] = lhs[0]*rhs[0] + lhs[1]*rhs[4] + lhs[2]*rhs[8] + lhs[3]*rhs[12];
  temp[1] = lhs[0]*rhs[1] + lhs[1]*rhs[5] + lhs[2]*rhs[9] + lhs[3]*rhs[13];
  temp[2] = lhs[0]*rhs[2] + lhs[1]*rhs[6] + lhs[2]*rhs[10] + lhs[3]*rhs[14];
  temp[3] = lhs[0]*rhs[3] + lhs[1]*rhs[7] + lhs[2]*rhs[11] + lhs[3]*rhs[15];
  temp[4] = lhs[4]*rhs[0] + lhs[5]*rhs[4] + lhs[6]*rhs[8] + lhs[7]*rhs[12];
  temp[5] = lhs[4]*rhs[1] + lhs[5]*rhs[5] + lhs[6]*rhs[9] + lhs[7]*rhs[13];
  temp[6] = lhs[4]*rhs[2] + lhs[5]*rhs[6] + lhs[6]*rhs[10] + lhs[7]*rhs[14];
  temp[7] = lhs[4]*rhs[3] + lhs[5]*rhs[7] + lhs[6]*rhs[11] + lhs[7]*rhs[15];
  temp[8] = lhs[8]*rhs[0] + lhs[9]*rhs[4] + lhs[10]*rhs[8] + lhs[11]*rhs[12];
  temp[9] = lhs[8]*rhs[1] + lhs[9]*rhs[5] + lhs[10]*rhs[9] + lhs[11]*rhs[13];
  temp[10] = lhs[8]*rhs[2] + lhs[9]*rhs[6] + lhs[10]*rhs[10] + lhs[11]*rhs[14];
  temp[11] = lhs[8]*rhs[3] + lhs[9]*rhs[7] + lhs[10]*rhs[11] + lhs[11]*rhs[15];
  temp[12] = lhs[12]*rhs[0] + lhs[13]*rhs[4] + lhs[14]*rhs[8] + lhs[15]*rhs[12];
  temp[13] = lhs[12]*rhs[1] + lhs[13]*rhs[5] + lhs[14]*rhs[9] + lhs[15]*rhs[13];
  temp[14] = lhs[12]*rhs[2] + lhs[13]*rhs[6] + lhs[14]*rhs[10] + lhs[15]*rhs[14];
  temp[15] = lhs[12]*rhs[3] + lhs[13]*rhs[7] + lhs[14]*rhs[11] + lhs[15]*rhs[15];
  memcpy( res, &temp, sizeof(res) );
}

GLvoid ShaderCamera::cross( vec3& res, const vec3& lhs, const vec3& rhs ) {
  vec3 temp;
  temp[0] = lhs[1] * rhs[2] - rhs[1] * lhs[2];
  temp[1] = lhs[0] * rhs[2] - rhs[0] * lhs[2];
  temp[2] = lhs[0] * rhs[1] - rhs[0] * lhs[1];
  memcpy( res, &temp, sizeof(res) );
}

GLvoid ShaderCamera::print() {
  cout << "ShaderCamera:" << endl;
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
