#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glTypes.h"
#include "useful_things.h"
#include "GLScene.h"
#include "ShaderScene.h"
#include <iostream>

using namespace std;

ShaderScene shaderScene( 3 );
GLScene glScene( 3 );

GLvoid redisplay();
GLvoid reshapeGL( GLint width, GLint height );

GLvoid displayGL() {
  glutSetWindow( glScene.m_windowID );
  glEnable( GL_DEPTH_TEST );
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glScene.drawCurrentModel();

  glutSwapBuffers();
}

GLvoid reshapeGL( GLint width, GLint height ) {
  glutSetWindow( glScene.m_windowID );
  glScene.m_windowWidth = width;
  glScene.m_windowHeight = height;

  glViewport( 0, 0, width, height );

  glScene.m_cam->updatePerspective( (GLfloat)width / (GLfloat)height );

  //glScene.print();
  

  glutPostRedisplay();
}

GLvoid displayShader() {
  glutSetWindow( shaderScene.m_windowID );
  glEnable( GL_DEPTH_TEST );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  shaderScene.drawCurrentModel();
  glutSwapBuffers();
}

GLvoid reshapeShader( GLint width, GLint height ) {
  glutSetWindow( shaderScene.m_windowID );
  shaderScene.m_windowWidth = width;
  shaderScene.m_windowHeight = height;

  glViewport( 0, 0, width, height );

  shaderScene.m_cam->updatePerspective( (GLfloat)width / (GLfloat)height );

  glutPostRedisplay();
}

GLvoid press( GLubyte key, GLint x, GLint y) { 
  glutSetWindow( glScene.m_windowID );
  glScene.m_keyboard->press( key, x, y ); 
  glScene.handleKeys(); 

  glutSetWindow( shaderScene.m_windowID );
  shaderScene.m_keyboard->press( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid release( GLubyte key, GLint x, GLint y) { 
  glutSetWindow( glScene.m_windowID );
  glScene.m_keyboard->release( key, x, y ); 
  glScene.handleKeys(); 

  glutSetWindow( shaderScene.m_windowID );
  shaderScene.m_keyboard->release( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid specialPress( GLint key, GLint x, GLint y) { 
  glutSetWindow( glScene.m_windowID );
  glScene.m_keyboard->specialPress( key, x, y ); 
  glScene.handleKeys(); 

  glutSetWindow( shaderScene.m_windowID );
  shaderScene.m_keyboard->specialPress( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid specialRelease( GLint key, GLint x, GLint y) { 
  glutSetWindow( glScene.m_windowID );
  glScene.m_keyboard->specialRelease( key, x, y ); 
  glScene.handleKeys(); 

  glutSetWindow( shaderScene.m_windowID );
  shaderScene.m_keyboard->specialRelease( key, x, y );   
  shaderScene.handleKeys();

  redisplay();
}

GLvoid redisplay() {
  glutSetWindow( glScene.m_windowID );
  glutPostRedisplay();
  //displayGL();
  glutSetWindow( shaderScene.m_windowID );
  glutPostRedisplay();
  //displayShader();
}

// Set light position, default color, read model data, etc.
GLvoid init() {
  // Initialize the GL rendered scene
  glutSetWindow( glScene.m_windowID );
  glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
  glScene.m_modChoice = 0;
  //glScene.m_rMode = R_WIREFRAME;
  //glScene.m_color->setColor( 1.0f, 1.0f, 1.0f, 1.0f );
  glScene.m_ccw = GL_TRUE;
  glScene.m_backfaceCulling = GL_TRUE;
  glScene.m_models[0].read( "../models/cow_up.in" );
  glScene.m_models[1].read( "../models/phone.in" );
  glScene.m_models[2].read( "../models/cube.in" );
  glScene.centerOnCurrentModel(); 
  glScene.setDefaultLights();

  // Initialize the shader rendered scene
  glutSetWindow( shaderScene.m_windowID );
  glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
  shaderScene.m_modChoice = 0;
  //shaderScene.m_rMode = R_WIREFRAME;
  //shaderScene.m_color->setColor( 1.0f, 1.0f, 1.0f, 1.0f );
  shaderScene.m_ccw = GL_TRUE;
  glScene.m_backfaceCulling = GL_TRUE;
  shaderScene.m_models[0].read( "../models/cow_up.in" );
  shaderScene.m_models[1].read( "../models/phone.in" );
  shaderScene.m_models[2].read( "../models/cube.in" );
  shaderScene.loadDefaultShaders();
  shaderScene.centerOnCurrentModel(); 
  shaderScene.setDefaultLights();
}

GLint main( int argc, char** argv )
{
  glScene.print();
  shaderScene.print();
  
  // Initialize GLUT 
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
  glutInitWindowPosition( 100, 100 );
  glutInitWindowSize( glScene.m_windowWidth, glScene.m_windowHeight );
  glScene.m_windowID = glutCreateWindow( "CS6366 Program 3 - OpenGL Rendering" );
  glutSetWindow( glScene.m_windowID );
  glutDisplayFunc( displayGL );
  glutReshapeFunc( reshapeGL );
  glutKeyboardFunc( press );
  glutKeyboardUpFunc( release );
  glutSpecialFunc( specialPress );
  glutSpecialUpFunc( specialRelease );  

  glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
  glutInitWindowPosition( 800, 100 );
  glutInitWindowSize( shaderScene.m_windowWidth, shaderScene.m_windowHeight );
  shaderScene.m_windowID = glutCreateWindow( "CS6366 Program 3 - Shader Rendering" );
  glutSetWindow( shaderScene.m_windowID );
  glutDisplayFunc( displayShader );
  glutReshapeFunc( reshapeShader );
  glutKeyboardFunc( press );
  glutKeyboardUpFunc( release );
  glutSpecialFunc( specialPress );
  glutSpecialUpFunc( specialRelease );

  // Must check support sometime after first window is created.
  if ( !glScene.checkSupport() || !shaderScene.checkSupport() ) {
    exit(1);
  }

  init();

  glScene.print();
  shaderScene.print();

  glutMainLoop();

  return 0;
}