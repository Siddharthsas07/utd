#ifndef SHADER_CAMERA_H
#define SHADER_CAMERA_H

#include "Camera.h"

using namespace std;

class ShaderCamera : public Camera {
private:
  GLvoid init();

  GLvoid setFrustum( GLfloat l, GLfloat r, GLfloat b, 
                     GLfloat t, GLfloat n, GLfloat f );

  GLvoid identity( mat4& m );
  GLvoid transpose( mat4& m );
  GLvoid multMatrix( mat4& res, const mat4& lhs,const  mat4& rhs );
  GLvoid translate( mat4& m, GLfloat x, GLfloat y, GLfloat z );
  GLvoid rotateU( mat4& m, GLfloat theta );
  GLvoid rotateV( mat4& m, GLfloat theta );
  GLvoid rotateN( mat4& m, GLfloat theta );
  GLvoid normalize( vec3& v );  
  GLvoid cross( vec3& res, const vec3& lhs, const vec3& rhs );

public:
  ShaderCamera();
  ~ShaderCamera();  

  // Translate the camera (overloads)
  GLvoid translate( GLfloat x, GLfloat y, GLfloat z );  
  GLvoid translate( const vec3& v );
  GLvoid translate( GLubyte axis, GLint dir );

  // Rotate the camera (overloads)
  GLvoid rotate( const vec3& v, GLfloat theta );
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