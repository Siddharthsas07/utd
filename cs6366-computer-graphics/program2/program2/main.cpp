#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glTypes.h"
#include "useful_things.h"
#include "Scene.h"
#include "Window.h"
#include <iostream>

using namespace std;

Scene scene( 3 );

// Window ID numbers
GLuint IDmainWindow;

GLvoid redisplay();

GLvoid mainWindowDisplay() {
  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  scene.drawCurrentModel(); 

  glutSwapBuffers();
}

GLvoid mainWindowReshape( GLint width, GLint height ) {
  windowWidth = width;
  windowHeight = height;

  glViewport( 0, 0, width, height );

  scene.m_cam->updatePerspective( (GLfloat)width / (GLfloat)height );

  //scene.drawCurrentModel();

  redisplay();
}

GLvoid mainWindowMenu( GLint selection ) {    
  if ( selection == 1 ) {
    scene.m_modChoice = 0;
    scene.centerOnCurrentModel();
  } 

  else if ( selection == 2 ) {
    scene.m_modChoice = 1;
    scene.centerOnCurrentModel();
  }

  else if ( selection == 3 ) {
    scene.m_modChoice = 2;
    scene.centerOnCurrentModel();
  }

  else if ( selection == 4 ) { scene.m_rMode = R_POINT; }
  else if ( selection == 5 ) { scene.m_rMode = R_WIREFRAME; }
  else if ( selection == 6 ) { scene.m_rMode = R_SOLID; }
  else if ( selection == 7 ) { scene.m_ccw = GL_TRUE; }
  else if ( selection == 8 ) { scene.m_ccw = GL_FALSE; }

  else if ( selection == 9 ) {
    scene.m_backfaceCulling = !scene.m_backfaceCulling;
  }
  
  redisplay();
}

GLvoid press( GLubyte key, GLint x, GLint y) { 
  scene.m_keyboard->press( key, x, y ); 
  scene.handleKeys();
  redisplay();
}

GLvoid release( GLubyte key, GLint x, GLint y) { 
  scene.m_keyboard->release( key, x, y ); 
  scene.handleKeys();
  redisplay();
}

GLvoid specialPress( GLint key, GLint x, GLint y) { 
  scene.m_keyboard->specialPress( key, x, y ); 
  scene.handleKeys();
  redisplay();
}

GLvoid specialRelease( GLint key, GLint x, GLint y) { 
  scene.m_keyboard->specialRelease( key, x, y ); 
  scene.handleKeys();
  redisplay();
}

GLvoid redisplay() {
  glutPostRedisplay();
}

GLint main( int argc, char** argv )
{
  scene.print();
  
  // Initialize GLUT 
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
  glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
  glEnable(GL_DEPTH_TEST);

  IDmainWindow = glutCreateWindow( "CS6366 Program 2" );
  glutReshapeFunc( mainWindowReshape );
  glutDisplayFunc( mainWindowDisplay );
  glutKeyboardFunc( press );
  glutKeyboardUpFunc( release );
  glutSpecialFunc( specialPress );
  glutSpecialUpFunc( specialRelease );

  // Must check support sometime after first window is created.
  if ( !scene.checkSupport() ) {
    exit(1);
  }

  glutCreateMenu( mainWindowMenu );
  glutAddMenuEntry( "Model:", 0 );
  glutAddMenuEntry( "+Cow", 1 );
  glutAddMenuEntry( "+Phone", 2 );
  glutAddMenuEntry( "+Cube", 3 );
  glutAddMenuEntry( "Rendering Mode:", 0 );
  glutAddMenuEntry( "+Points", 4 );
  glutAddMenuEntry( "+Wireframe", 5 );
  glutAddMenuEntry( "+Solid", 6 );
  glutAddMenuEntry( "Drawing Order:", 0 );
  glutAddMenuEntry( "+Counterclockwise", 7 );
  glutAddMenuEntry( "+Clockwise", 8 );
  glutAddMenuEntry( "+Enable/Disable Culling", 9 );
  glutAttachMenu( GLUT_RIGHT_BUTTON );
  
  scene.m_models[0].read( "../models/cow_up.in" );
  scene.m_models[1].read( "../models/phone.in" );
  scene.m_models[2].read( "../models/cube.in" );

  scene.m_modChoice = 0;
  scene.m_rMode = R_WIREFRAME;
  scene.m_color->setColor( 1.0f, 1.0f, 1.0f );
  scene.m_ccw = GL_TRUE;
  scene.m_backfaceCulling = GL_TRUE;

  scene.loadDefaultShaders();

  scene.centerOnCurrentModel(); 

  scene.print();

  glutMainLoop();

  return 0;
}