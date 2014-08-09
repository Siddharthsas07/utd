#include "Scene.h"

using namespace std;

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

GLvoid Scene::setDefaultLights() {
  m_lightsEnabled = GL_TRUE;
  m_smoothShading = GL_TRUE;
  m_globalAmbient[0] = 0.1f; m_globalAmbient[1] = 0.1f; m_globalAmbient[2] = 0.1f; m_globalAmbient[3] = 1.0f;
  m_light->setAmbient( 0.2f, 0.2f, 0.2f, 1.0f );
  m_light->setDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
  m_light->setSpecular( 0.1f, 0.1f, 0.1f, 1.0f );
  m_light->setPosition( 0.0f, 0.0f, 0.0f, 1.0f );
  // RESETTING COLORS WORKS IF YOU DON'T SET TH EPOSITION LOLOLOLOL
  // By setting the light position here and not explicitly altering it later, 
  // it will always be at the camera's position
  glLightfv( GL_LIGHT0, GL_POSITION, m_light->m_pos );
  glLightfv( GL_LIGHT0, GL_AMBIENT, m_light->m_amb );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, m_light->m_dif );
  glLightfv( GL_LIGHT0, GL_SPECULAR, m_light->m_spec );
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, m_globalAmbient );

  glEnable( GL_NORMALIZE );
  glEnable( GL_DEPTH_TEST );
}

GLvoid Scene::clamp( GLfloat& f ) {
  if ( f < 0.0f ) {
    f = 0.0f;
  }
  else if ( f > 1.0f ) {
    f = 1.0f;
  }
}

GLvoid Scene::clamp( col4f& f ) {
  if ( f[0] < 0.0f ) {
    f[0] = 0.0f;
  }
  else if ( f[0] > 1.0f ) {
    f[0] = 1.0f;
  }
  if ( f[1] < 0.0f ) {
    f[1] = 0.0f;
  }
  else if ( f[1] > 1.0f ) {
    f[1] = 1.0f;
  }
  if ( f[2] < 0.0f ) {
    f[2] = 0.0f;
  }
  else if ( f[2] > 1.0f ) {
    f[2] = 1.0f;
  }
  if ( f[3] < 0.0f ) {
    f[3] = 0.0f;
  }
  else if ( f[3] > 1.0f ) {
    f[3] = 1.0f;
  }
}
