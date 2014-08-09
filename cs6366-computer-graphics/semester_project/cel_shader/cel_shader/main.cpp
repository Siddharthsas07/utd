#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glTypes.h"
#include "useful_things.h"
#include "ShaderScene.h"
#include <iostream>

using namespace std;

ShaderScene shaderScene( 3 );

GLvoid redisplay();

GLvoid displayShader() {
  glEnable( GL_DEPTH_TEST );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  shaderScene.drawCurrentModel();
  glutSwapBuffers();
}

GLvoid reshapeShader( GLint width, GLint height ) {
  shaderScene.m_windowWidth = width;
  shaderScene.m_windowHeight = height;

  glViewport( 0, 0, width, height );

  shaderScene.m_cam->updatePerspective( (GLfloat)width / (GLfloat)height );

  glutPostRedisplay();
}

GLvoid press( GLubyte key, GLint x, GLint y) {
  shaderScene.m_keyboard->press( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid release( GLubyte key, GLint x, GLint y) { 
  shaderScene.m_keyboard->release( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid specialPress( GLint key, GLint x, GLint y) { 
  shaderScene.m_keyboard->specialPress( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid specialRelease( GLint key, GLint x, GLint y) { 
  shaderScene.m_keyboard->specialRelease( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid redisplay() {
  glutPostRedisplay();
}

// Set light position, default color, read model data, etc.
GLvoid init() {
  // Initialize the shader rendered scene
  glutSetWindow( shaderScene.m_windowID );
  glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
  shaderScene.m_modChoice = 0;
  shaderScene.m_ccw = GL_TRUE;
  shaderScene.m_models[0].read( "../models/cow_up.in" );
  shaderScene.m_models[1].read( "../models/cube.in" );
  shaderScene.m_models[2].read( "../models/phone.in" );
  shaderScene.loadDefaultShaders();
  shaderScene.centerOnCurrentModel(); 
  shaderScene.setDefaultLights();
}

GLint main( int argc, char** argv )
{
  shaderScene.print();
  
  // Initialize GLUT 
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
  glutInitWindowPosition( 100, 100 );
  glutInitWindowSize( shaderScene.m_windowWidth, shaderScene.m_windowHeight );
  shaderScene.m_windowID = glutCreateWindow( "CS6366 Program 3 - Shader Rendering" );
  glutDisplayFunc( displayShader );
  glutReshapeFunc( reshapeShader );
  glutKeyboardFunc( press );
  glutKeyboardUpFunc( release );
  glutSpecialFunc( specialPress );
  glutSpecialUpFunc( specialRelease );

  // Must check support sometime after first window is created.
  if ( !shaderScene.checkSupport() ) {
    exit(1);
  }

  init();

  shaderScene.print();
  shaderScene.m_models[1].print();

  glutMainLoop();

  return 0;
}