#ifndef GL_CAMERA_H
#define GL_CAMERA_H

#include "Camera.h"

using namespace std;

class GLCamera : public Camera {
private:
  GLvoid init();

protected:
  GLvoid rotateU( GLfloat theta );
  GLvoid rotateV( GLfloat theta );
  GLvoid rotateN( GLfloat theta );
  
public:
  GLCamera();
  ~GLCamera();  

  // Translate the camera (overloads)
  GLvoid translate( GLfloat x, GLfloat y, GLfloat z );  
  GLvoid translate( const vec3f& v );
  GLvoid translate( GLubyte axis, GLint dir );

  // Rotate the camera (overloads)
  GLvoid rotate( const vec3f& v, GLfloat theta );
  GLvoid rotate( GLubyte axis, GLfloat theta );
  GLvoid rotate( GLubyte axis, GLint dir );

  // Alter near and far clipping planes
  GLvoid moveClippingPlane( GLubyte plane, GLint dir );

  // Recaulate perspectives and focus on chosen model.
  GLvoid centerOn( const Model& m, GLint width, GLint height );
  GLvoid lookAt( GLfloat ex, GLfloat ey, GLfloat ez,
                 GLfloat cx, GLfloat cy, GLfloat cz,
                 GLfloat ux, GLfloat uy, GLfloat uz );

  // Calculate new perspective matrix 
  GLvoid updatePerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar );
  GLvoid updatePerspective( GLfloat aspect );

  GLvoid print();
};

#endif