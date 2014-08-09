#ifndef SCENE_H
#define SCENE_H

#include "glTypes.h"
#include "Model.h"
#include "Camera.h"

using namespace std;

// Supported rendering modes
enum RENDER_MODE {
  R_POINT, R_WIREFRAME, R_SOLID
};

class Scene {
private:
  GLdouble COLOR_CHANGE;

public:
  Scene();
  Scene( GLuint numModels );
  ~Scene();

  // Draw the model
  GLvoid drawCurrentModel();

  // Set up initial view of model
  GLvoid centerCameraOnCurrentModel();
  GLvoid calcPerspectiveForCurrentModel();
  GLvoid calcTranslateStepsForCurrentModel();

  // Setting of color (overloads)
  GLvoid setColor( col4d* c );
  GLvoid setColor( col3d* c );
  GLvoid setColor( GLdouble r, GLdouble g, GLdouble b, GLdouble a );
  GLvoid setColor( GLdouble r, GLdouble g, GLdouble b );

  // Change color value (r,g,b,a) in dir direction(pos/neg)
  GLvoid changeColor( GLubyte value, GLbyte dir );

  // Print statics about scene to console
  GLvoid print();

  Camera* m_cam;               // The user's camera

  Model* m_models;             // Array of models loaded
  GLuint m_numModels;          // Number of models loaded
  GLuint m_modChoice;          // Currently selected model for drawing
  col4d  m_rColor;             // Current color for drawing
  GLdouble m_rColorDelta;      // Increment/decrement value for default color changes
  RENDER_MODE m_rMode;         // Current rendering mode for drawing
  GLboolean m_ccw;             // Draw in counter-clockwise?
  GLboolean m_backfaceCulling; // Enable/disable culling
};

#endif