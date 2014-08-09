#ifndef SHADER_CAMERA_H
#define SHADER_CAMERA_H

#include "Camera.h"

using namespace std;

class ShaderCamera : public Camera {
private:
  GLvoid init();

  GLvoid setFrustum( GLfloat l, GLfloat r, GLfloat b, 
                     GLfloat t, GLfloat n, GLfloat f );

  GLvoid identity( mat4f& m );
  GLvoid transpose( mat4f& m );
  GLvoid multMatrix( mat4f& res, const mat4f& lhs,const  mat4f& rhs );
  GLvoid translate( mat4f& m, GLfloat x, GLfloat y, GLfloat z );
  GLvoid rotateU( mat4f& m, GLfloat theta );
  GLvoid rotateV( mat4f& m, GLfloat theta );
  GLvoid rotateN( mat4f& m, GLfloat theta );
  GLvoid normalize( vec3f& v );  
  GLvoid cross( vec3f& res, const vec3f& lhs, const vec3f& rhs );

public:
  ShaderCamera();
  ~ShaderCamera();  

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