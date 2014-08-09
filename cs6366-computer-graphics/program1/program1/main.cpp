#include "glTypes.h"
#include "useful_things.h"
#include "GL/freeglut.h"
//#include "GL/glui.h"
#include "Scene.h"
#include "Window.h"
#include <iostream>

using namespace std;

Scene scene( 3 );

// Derpalicious
//GLUI *glui;

// Window ID numbers
GLuint IDmainWindow;
GLuint IDviewWindow;
GLuint IDcmdWindow;

col4f colors[3] = 
{
{0.5, 0.0, 0.0, 0.0}, /*main window BG color*/
{0.0, 0.0, 0.0, 0.0}, /*sub window BG color*/
{1.0, 1.0, 1.0, 1.0}  /*default model color*/
};

// Color indices for windows
GLuint mainWindowColor = 0;
GLuint viewWindowColor = 1;
GLuint cmdWindowColor = 1;

GLvoid redisplayAll();

GLvoid mainWindowDisplay() {
  glClearColor( colors[mainWindowColor][0], colors[mainWindowColor][1], 
    colors[mainWindowColor][2], colors[mainWindowColor][3] );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glutSwapBuffers();
}

GLvoid mainWindowReshape( GLint width, GLint height ) {
  windowWidth = width;
  windowHeight = height;

  glViewport( 0, 0, windowWidth, windowHeight );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, windowWidth, windowHeight, 0 );
  glMatrixMode( GL_MODELVIEW );
    
  subwindowWidth = (GLuint)((windowWidth - (3*BORDER_SIZE)) / 2.0f);
  subwindowHeight = (GLuint)(windowHeight - (2*BORDER_SIZE));

  // Position model view window
  glutSetWindow( IDviewWindow );
  glutPositionWindow( BORDER_SIZE, BORDER_SIZE );
  glutReshapeWindow( subwindowWidth, subwindowHeight );
  
  // Position command window
  glutSetWindow( IDcmdWindow);
  glutPositionWindow( BORDER_SIZE*2+subwindowWidth, BORDER_SIZE );
  glutReshapeWindow( subwindowWidth, subwindowHeight );
}

GLvoid mainWindowKeyboard( GLubyte key, GLint x, GLint y ) {
  // TODO: keyboard buffer
  if ( key == 27 ) { exit(0); }
}

GLvoid viewWindowDisplay() {
  glClearColor( colors[viewWindowColor][0], colors[viewWindowColor][1], 
    colors[viewWindowColor][2], colors[viewWindowColor][3] );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  scene.drawCurrentModel(); 

  glutSwapBuffers();
}

GLvoid viewWindowReshape( GLint width, GLint height) {
  subwindowWidth = width;
  subwindowHeight = height;

  glViewport( 0, 0, width, height );

  scene.m_cam->calcPerspective( width, height /*&(scene.m_models[scene.m_modChoice])*/ );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();  
  gluPerspective( scene.m_cam->m_perspective[0], scene.m_cam->m_perspective[1], 
    scene.m_cam->m_perspective[2], scene.m_cam->m_perspective[3] );
  glGetDoublev( GL_PROJECTION_MATRIX, scene.m_cam->m_projectionMatrix );
  glMatrixMode( GL_MODELVIEW );

  glutPostRedisplay();
}

GLvoid viewWindowMenu( GLint selection ) {    
  if ( selection == 1 ) {
    scene.m_modChoice = 0;
    scene.centerCameraOnCurrentModel();
  } 

  else if ( selection == 2 ) {
    scene.m_modChoice = 1;
    scene.centerCameraOnCurrentModel();
  }

  else if ( selection == 3 ) {
    scene.m_modChoice = 2;
    scene.centerCameraOnCurrentModel();
  }

  else if ( selection == 4 ) { scene.m_rMode = R_POINT; }
  else if ( selection == 5 ) { scene.m_rMode = R_WIREFRAME; }
  else if ( selection == 6 ) { scene.m_rMode = R_SOLID; }
  else if ( selection == 7 ) { scene.m_ccw = GL_TRUE; }
  else if ( selection == 8 ) { scene.m_ccw = GL_FALSE; }

  else if ( selection == 9 ) {
    scene.m_backfaceCulling = !scene.m_backfaceCulling;
  }
  
  redisplayAll();
}

GLvoid viewWindowKeyboard( GLubyte key, GLint x, GLint y ) {
  if ( key == 27 ) { exit(0); }

  // Reset view
  if ( key == 'r' || key == 'R' ) { scene.centerCameraOnCurrentModel(); }
  // Translation along specific axes by 5% of model size
  if ( key == 'a' || key == 'A' ) { scene.m_cam->translate( 'u', 1 ); }
  if ( key == 'd' || key == 'D' ) { scene.m_cam->translate( 'u', -1 ); }
  if ( key == 'w' || key == 'W' ) { scene.m_cam->translate( 'n', 1 ); }
  if ( key == 's' || key == 'S' ) { scene.m_cam->translate( 'n', -1 ); }
  if ( key == 'q' || key == 'Q' ) { scene.m_cam->translate( 'v', 1 ); }
  if ( key == 'e' || key == 'E' ) { scene.m_cam->translate( 'v', -1 ); }
  if ( key == 'z' || key == 'Z' ) { scene.m_cam->translate(5.0, 5.0, 5.0); }
  // Rotation about specific axes by 10 degrees
  if ( key == 'l' || key == 'L' ) { scene.m_cam->rotate('v', 1 ); }
  if ( key == '\'' )              { scene.m_cam->rotate('v', -1 ); }
  if ( key == 'p' || key == 'P' ) { scene.m_cam->rotate('u', 1 );  }
  if ( key == ';' )               { scene.m_cam->rotate('u', -1 ); }
  if ( key == 'o' || key == 'O' ) { scene.m_cam->rotate('n', 1 ); }
  if ( key == '[' )               { scene.m_cam->rotate('n', -1 ); }
  // Change drawing colors
  if ( key == '1' )               { scene.changeColor( 'r', 1 ); }
  if ( key == '2' )               { scene.changeColor( 'g', 1 ); }
  if ( key == '3' )               { scene.changeColor( 'b', 1 ); }
  if ( key == '4' )               { scene.changeColor( 'a', 1 ); }
  if ( key == '5' )               { scene.changeColor( 'r', -1 ); }
  if ( key == '6' )               { scene.changeColor( 'g', -1 ); }
  if ( key == '7' )               { scene.changeColor( 'b', -1 ); }
  if ( key == '8' )               { scene.changeColor( 'a', -1 ); }
  if ( key == '9' )               { scene.m_cam->moveClippingPlane( 'f', 1 ); }
  if ( key == '0' )               { scene.m_cam->moveClippingPlane( 'n', 1 ); }
  if ( key == '-' )               { scene.m_cam->moveClippingPlane( 'f', -1 ); }
  if ( key == '=' )               { scene.m_cam->moveClippingPlane( 'n', -1 ); }
  if ( key == ' ' ) {
    scene.m_modChoice = (scene.m_modChoice + 1) % scene.m_numModels;
    scene.centerCameraOnCurrentModel();
  }

  redisplayAll();
}

GLvoid cmdWindowDisplay()
{
  glClearColor( colors[cmdWindowColor][0], colors[cmdWindowColor][1], 
    colors[cmdWindowColor][2], colors[cmdWindowColor][3] );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glutSwapBuffers();
}

GLvoid cmdWindowReshape( GLint width, GLint height )
{
  subwindowWidth = width;
  subwindowHeight = height;

  glViewport( 0, 0, width, height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, windowWidth, windowHeight, 0 );
  glMatrixMode( GL_MODELVIEW );

  glutPostRedisplay();
}

GLvoid cmdWindowMenu( GLint selection )
{      
  if ( selection == 1 ) {
    scene.centerCameraOnCurrentModel();   
  } 

  else if ( selection == 27 ) { exit(0); }
  
  redisplayAll();
}

GLvoid redisplayAll()
{
  // Command window needs no reshaping.
  glutSetWindow( IDcmdWindow );
  cmdWindowReshape( subwindowWidth, subwindowHeight );
  glutPostRedisplay();

  // Reshape view of model before redisplaying.
  glutSetWindow( IDviewWindow );
  viewWindowReshape( subwindowWidth, subwindowHeight );
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

  IDmainWindow = glutCreateWindow( "CS6366 Program 1" );
  glutReshapeFunc( mainWindowReshape );
  glutDisplayFunc( mainWindowDisplay );
  glutKeyboardFunc( mainWindowKeyboard );

  IDviewWindow = glutCreateSubWindow( IDmainWindow, 
    BORDER_SIZE, BORDER_SIZE, 
    SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT );
  glutReshapeFunc( viewWindowReshape );
  glutDisplayFunc( viewWindowDisplay );
  glutKeyboardFunc( viewWindowKeyboard );
  glutCreateMenu( viewWindowMenu );
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

  // This window honestly seems pretty useless now. But I'm leaving it in because
  // I might need it later. 
  IDcmdWindow = glutCreateSubWindow( IDmainWindow, 
    BORDER_SIZE*2+SUBWINDOW_WIDTH, BORDER_SIZE, 
    SUBWINDOW_WIDTH, SUBWINDOW_HEIGHT );
  glutReshapeFunc( cmdWindowReshape );
  glutDisplayFunc( cmdWindowDisplay );
  glutKeyboardFunc( mainWindowKeyboard );
  glutCreateMenu( cmdWindowMenu );
  glutAddMenuEntry( "Reset parameters", 1 );
  glutAddMenuEntry( "Quit", 27 );
  glutAttachMenu( GLUT_RIGHT_BUTTON );
  
  scene.m_models[0].read( "../models/cow_up.in" );
  scene.m_models[1].read( "../models/phone.in" );
  scene.m_models[2].read( "../models/cube.in" );

  scene.m_modChoice = 0;
  scene.m_rMode = R_WIREFRAME;
  scene.m_rColor[0] = 1.0;
  scene.m_rColor[1] = 1.0;
  scene.m_rColor[2] = 1.0;
  scene.m_rColor[3] = 1.0;

  scene.m_ccw = GL_TRUE;
  scene.m_backfaceCulling = GL_TRUE;

  scene.centerCameraOnCurrentModel();
  
  glutMainLoop();

  return 0;
}