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
protected:
  GLvoid calcTranslateDelta( const Model& m );
  GLvoid calcRotateDelta( const Model& m );

  GLfloat FOVY;                  // Default FOVY in degrees
  GLfloat FOVX;                  // Default FOVX in degrees
  GLfloat ASPECT_RATIO;          // Default aspect ratio
  GLfloat NEAR_CLIPPING_PLANE;   // Default clipping plane distance
  GLfloat FAR_CLIPPING_PLANE;    // Default clipping plane distance
  GLfloat DEFAULT_TRANSLATE_STEP;// Default units translated per step
  GLfloat DEFAULT_ROTATE_ANGLE;  // Default degrees rotated per step

public:
  // Translate the camera (overloads)
  virtual GLvoid translate( GLfloat x, GLfloat y, GLfloat z ) = 0;  
  virtual GLvoid translate( const vec3f& v ) = 0;
  virtual GLvoid translate( GLubyte axis, GLint dir ) = 0;

  // Rotate the camera (overloads)
  virtual GLvoid rotate( const vec3f& v, GLfloat theta ) = 0;
  virtual GLvoid rotate( GLubyte axis, GLfloat theta ) = 0;
  virtual GLvoid rotate( GLubyte axis, GLint dir ) = 0;

  // Alter near and far clipping planes
  virtual GLvoid moveClippingPlane( GLubyte plane, GLint dir ) = 0;

  // Recaulate perspectives and focus on chosen model.
  virtual GLvoid centerOn( const Model& m, GLint width, GLint height ) = 0;
  virtual GLvoid lookAt( GLfloat ex, GLfloat ey, GLfloat ez,
                         GLfloat cx, GLfloat cy, GLfloat cz,
                         GLfloat ux, GLfloat uy, GLfloat uz ) = 0;

  // Calculate new perspective matrix 
  virtual GLvoid updatePerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar ) = 0;
  virtual GLvoid updatePerspective( GLfloat aspect ) = 0;

  virtual GLvoid print() = 0;

  GLboolean m_invertYLook;  // Invert Y-axis control on camera?
  GLboolean m_invertXLook;  // Invert X-axis control on camera?
  GLfloat m_fovY;           // Current FOV_y
  GLfloat m_fovX;           // Current FOV_x
  GLfloat m_aspect;         // Current aspect ratio
  GLfloat m_zNear;          // Current clipping plane
  GLfloat m_zFar;           // Current clipping plane
  GLfloat m_clipDelta;      // Default change for plane inc/decrement

  GLfloat m_pos[4];         // Position in world coords
  
  vec3f m_translateDelta;   // Default step distances for a translation in u=[0],v=[1],n=[2] directions
  vec3f m_rotateDelta;      // Default degrees for a rotation about u=[0],v=[1],n=[2] axes

  mat4f m_projMatr;         // The projection matrix
  mat4f m_modelViewMatr;    // The modelview matrix
  mat4f m_tempMatr;         // Temp variable for ops
};

#endif