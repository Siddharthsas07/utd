#include "Scene.h"

using namespace std;

Scene::Scene() {}

Scene::~Scene() {
  for ( vector<Light*>::iterator it = m_lights.begin(); it != m_lights.end(); it++ ) {
    delete *it;
  }
}

GLboolean Scene::checkSupport() {
  GLenum glewInitCheck = glewInit();

  if ( !(glewInitCheck == GLEW_OK) ) {
    printf( "[FAIL] %s\n", glewGetErrorString(glewInitCheck) );
    return false;
  }

  if ( !glewIsSupported("GL_VERSION_2_0") ) {
		printf( "[FAIL] OpenGL 2.0 not supported.\n" );
		return false;
  }

  printf( "[OK] GLEW Version: %s\n", glewGetString(GLEW_VERSION) );
  printf( "[OK] OpenGL Version: 2.0\n" );
  return true;
}

GLvoid Scene::clamp( GLfloat& f ) {
  f = fun::bound( f, 0.0f, 1.0f );
}

GLvoid Scene::clamp( col4& f ) {
  f[0] = fun::bound( f[0], 0.0f, 1.0f );
  f[1] = fun::bound( f[1], 0.0f, 1.0f );
  f[2] = fun::bound( f[2], 0.0f, 1.0f );
  f[3] = fun::bound( f[3], 0.0f, 1.0f );
}
