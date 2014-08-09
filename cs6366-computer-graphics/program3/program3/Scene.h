#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "glTypes.h"
#include "Camera.h"
#include "Keyboard.h"
#include "Model.h"
#include "Colorizer.h"
#include "Light.h"
#include "Window.h"
#include "textfile.h"

using namespace std;

// Supported rendering modes
enum RENDER_MODE {
  R_POINT, R_WIREFRAME, R_SOLID
};

class Scene {
protected:
  GLvoid clamp( GLfloat& f );
  GLvoid clamp( col4f& f );

public:
  GLboolean checkSupport();

  virtual GLvoid drawCurrentModel() = 0;
  virtual GLvoid centerOnCurrentModel() = 0;
  virtual GLvoid handleKeys() = 0;
  virtual GLvoid print() = 0;

  virtual GLvoid setDefaultLights() = 0;

  GLint m_windowWidth;         // Width of window this scene represents
  GLint m_windowHeight;        // Height of window this scene represents
  GLint m_windowID;            // OpenGL window ID for this window

  Camera* m_cam;               // Camera to move around the scene
  Keyboard* m_keyboard;        // The user's keyboard

  Model* m_models;             // Array of models loaded
  GLuint m_numModels;          // Number of models loaded
  GLuint m_modChoice;          // Currently selected model for drawing
  
  Colorizer* m_color;          // Colors

  Light* m_light;              // A single light source
  col4f m_globalAmbient;       // A single light for the global ambient
  GLfloat m_lightDelta;
  GLboolean m_lightsEnabled;   // Lights on?
  GLboolean m_smoothShading;   // Smooth shading?

  RENDER_MODE m_rMode;         // Current rendering mode for drawing
  GLboolean m_ccw;             // Draw in counter-clockwise?
  GLboolean m_backfaceCulling; // Backface culling on?
};

#endif