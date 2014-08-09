#ifndef CAMERA_H
#define CAMERA_H

#include "glTypes.h"
#include "GL/freeglut.h"
#include "Model.h"

using namespace std;

class Camera {
private:
  GLdouble FOVY;                  // Default FOVY in degrees
  GLdouble FOVX;                  // Default FOVX in degrees
  GLdouble ASPECT_RATIO;          // Default aspect ratio
  GLdouble NEAR_CLIPPING_PLANE;   // Default clipping plane distance
  GLdouble FAR_CLIPPING_PLANE;    // Default clipping plane distance
  GLdouble DEFAULT_TRANSLATE_STEP;// Default units translated per step
  GLdouble DEFAULT_ROTATE_ANGLE;  // Default degrees rotated per step

public:
  Camera();
  ~Camera();
  GLvoid initializeMembers();

  // Translate the camera (overloads)
  GLvoid translate( GLfloat x, GLfloat y, GLfloat z );  
  GLvoid translate( GLdouble x, GLdouble y, GLdouble z );
  GLvoid translate( vec3f* v );
  GLvoid translate( vec3d* v );
  GLvoid translate( GLubyte axis, GLint dir );

  // Rotate the camera (overloads)
  GLvoid rotate( vec3f* v, GLfloat theta );
  GLvoid rotate( vec3d* v, GLdouble theta );
  GLvoid rotate( GLubyte axis, GLfloat theta );
  GLvoid rotate( GLubyte axis, GLdouble theta );
  GLvoid rotate( GLubyte axis, GLint dir );

  // Alter near and far clipping planes
  GLvoid moveClippingPlane( GLubyte plane, GLint dir );

  // Recaulate perspectives and focus on chosen model.
  GLvoid centerOn( Model* m );
  GLvoid calcTranslateSteps( Model* m );
  GLvoid calcRotateAngles( Model* m );

  // Adjust parameters for gluPerspective
  GLvoid calcPerspective( GLint width, GLint height ); 
  GLvoid execPerspectiveChange();

  GLvoid print();

  GLboolean m_invertYLook;      // Invert Y-axis control on camera?
  GLboolean m_invertXLook;      // Invert X-axis control on camera?
  GLdouble m_fovY;              // Current FOV_y
  GLdouble m_fovX;              // Current FOV_x
  GLdouble m_aspectRatio;       // Current aspect ratio
  GLdouble m_nearClippingPlane; // Current clipping plane
  GLdouble m_farClippingPlane;  // Current clipping plane
  GLdouble m_clippingPlaneDelta;// Default change for plane inc/decrement

  col4d m_perspective;          // Holds values for a gluPerspective() call
  mat3d m_lookAt;               // Holds values for a gluLookAt() call
  
  vec3d m_userTranslation;      // Values of the last translation made
  vec3d m_translateStep;        // Default step distances for a translation in u=[0],v=[1],n=[2] directions

  vec3d m_userRotation;         // Values of the last rotation made
  vec3d m_rotateStep;           // Default degrees for a rotation about u=[0],v=[1],n=[2] axes

  mat4d m_projectionMatrix;     // The last projection matrix generated
  mat4d m_modelviewMatrix;      // The last modelview matrix generated
  mat4d m_resultantMatrix;      // Temp variable for ops

  mat4d m_inverseMatrix;        // Not used
  mat4d m_userMatrix;           // Not used
};

#endif