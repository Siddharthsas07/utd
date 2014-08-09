#ifndef GL_SCENE_H
#define GL_SCENE_H

#include "Scene.h"
#include "GLCamera.h"

class GLScene : public Scene {
private:
  GLvoid init();
  GLfloat COLOR_CHANGE;

public:
  GLScene();
  GLScene( GLuint numModels );
  ~GLScene();

  GLvoid drawCurrentModel();

  GLvoid centerOnCurrentModel();

  GLvoid handleKeys();

  GLvoid print();

  GLvoid setDefaultLights();
};

#endif