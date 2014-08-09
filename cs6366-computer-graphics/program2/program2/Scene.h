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
#include "Window.h"
#include "textfile.h"

using namespace std;

// Supported rendering modes
enum RENDER_MODE {
  R_POINT, R_WIREFRAME, R_SOLID
};

class Scene {
private:
  GLvoid init();
  GLfloat COLOR_CHANGE;

public:
  Scene();
  Scene( GLuint numModels );
  ~Scene();

  // Check for supported OpenGL version; Call after glutCreateWindow!
  GLboolean checkSupport();

  // Load shaders
  GLvoid loadDefaultShaders();
  GLint loadShader( char* loc, GLenum type );
  GLvoid printShaderInfoLog( GLuint shaderID );
  GLvoid printProgramInfoLog( GLuint programID );

  // Draw the model
  GLvoid drawCurrentModel();

  // Set up initial view of model
  GLvoid centerOnCurrentModel();

  GLvoid handleKeys();

  // Print statics about scene to console
  GLvoid print();

  GLint m_vShader;             // The vertex shader ID
  GLint m_fShader;             // The fragment shader ID
  GLint m_shaderProgram;       // The shader program ID  

  Camera* m_cam;               // The user's camera
  Keyboard* m_keyboard;        // The user's keyboard

  Model* m_models;             // Array of models loaded
  GLuint m_numModels;          // Number of models loaded
  GLuint m_modChoice;          // Currently selected model for drawing
  
  Colorizer* m_color;          // Colors

  RENDER_MODE m_rMode;         // Current rendering mode for drawing
  GLboolean m_ccw;             // Draw in counter-clockwise?
  GLboolean m_backfaceCulling; // Enable/disable culling
};

#endif