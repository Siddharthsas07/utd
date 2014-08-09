#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include "Window.h"
#include "glTypes.h"
#include "Model.h"

using namespace std;

class Camera {
private:
  GLvoid init();

  GLvoid setFrustum( GLfloat l, GLfloat r, GLfloat b, 
                     GLfloat t, GLfloat n, GLfloat f );
  GLvoid calcTranslateDelta( const Model& m );
  GLvoid calcRotateDelta( const Model& m );

  GLvoid identity( mat4f& m );
  GLvoid transpose( mat4f& m );
  GLvoid multMatrix( mat4f& res, const mat4f& lhs,const  mat4f& rhs );
  GLvoid translate( mat4f& m, GLfloat x, GLfloat y, GLfloat z );
  GLvoid rotateU( mat4f& m, GLfloat theta );
  GLvoid rotateV( mat4f& m, GLfloat theta );
  GLvoid rotateN( mat4f& m, GLfloat theta );
  GLvoid normalize( vec3f& v );  
  GLvoid cross( vec3f& res, const vec3f& lhs, const vec3f& rhs );
  
  GLfloat FOVY;                  // Default FOVY in degrees
  GLfloat FOVX;                  // Default FOVX in degrees
  GLfloat ASPECT_RATIO;          // Default aspect ratio
  GLfloat NEAR_CLIPPING_PLANE;   // Default clipping plane distance
  GLfloat FAR_CLIPPING_PLANE;    // Default clipping plane distance
  GLfloat DEFAULT_TRANSLATE_STEP;// Default units translated per step
  GLfloat DEFAULT_ROTATE_ANGLE;  // Default degrees rotated per step

public:
  Camera();
  ~Camera();  

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
  GLvoid centerOn( const Model& m );
  GLvoid lookAt( GLfloat ex, GLfloat ey, GLfloat ez,
                 GLfloat cx, GLfloat cy, GLfloat cz,
                 GLfloat ux, GLfloat uy, GLfloat uz );

  // Calculate new perspective matrix 
  GLvoid updatePerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar );
  GLvoid updatePerspective( GLfloat aspect );

  GLvoid print();

  GLboolean m_invertYLook;   // Invert Y-axis control on camera?
  GLboolean m_invertXLook;   // Invert X-axis control on camera?
  GLfloat m_fovY;           // Current FOV_y
  GLfloat m_fovX;           // Current FOV_x
  GLfloat m_aspect;         // Current aspect ratio
  GLfloat m_zNear;          // Current clipping plane
  GLfloat m_zFar;           // Current clipping plane
  GLfloat m_clipDelta;      // Default change for plane inc/decrement
  
  vec3f m_translateDelta;    // Default step distances for a translation in u=[0],v=[1],n=[2] directions
  vec3f m_rotateDelta;       // Default degrees for a rotation about u=[0],v=[1],n=[2] axes

  mat4f m_projMatr;          // The projection matrix
  mat4f m_modelViewMatr;     // The modelview matrix
  mat4f m_tempMatr;          // Temp variable for ops
};

#endif