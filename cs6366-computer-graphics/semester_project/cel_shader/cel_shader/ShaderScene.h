#ifndef SHADER_SCENE_H
#define SHADER_SCENE_H

#include "Scene.h"
#include "ShaderCamera.h"
#include "ShaderProgram.h"

class ShaderScene : public Scene {
private:
  GLvoid init();
  GLfloat COLOR_CHANGE;

  GLint globalAmbientLoc;
  GLint lightAmbientLoc;
  GLint lightDiffuseLoc;
  GLint lightSpecularLoc;
  GLint matAmbientLoc;
  GLint matDiffuseLoc;
  GLint matSpecularLoc;
  GLint matShininessLoc;
  GLint lightLocationLoc;
  GLint viewerLocationLoc;
  GLint vertexNormalLoc;
  GLint vertexPositionLoc;
  GLint lightsEnabledLoc;
  GLint modelViewLocationLoc;
  GLint projectionLocationLoc;
  GLint numLevelsLoc;
  GLint intervalPercentLoc;

  ShaderProgram m_shaderProgram;

protected:
  GLvoid pairwiseMultiply( col4& res, const col4& lhs, const GLfloat* rhs );

public:
  ShaderScene();
  ShaderScene( GLuint numModels );
  ~ShaderScene();

  GLvoid loadDefaultShaders();

  GLvoid drawCurrentModel();

  GLvoid centerOnCurrentModel();

  GLvoid handleKeys();

  GLvoid print();

  GLvoid setDefaultLights();  
};

#endif