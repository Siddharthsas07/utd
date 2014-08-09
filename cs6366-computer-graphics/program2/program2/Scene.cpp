#include "Scene.h"

using namespace std;

Scene::Scene() {
  m_numModels = 0; 
  init();
}

Scene::Scene( GLuint numModels ) {
  m_numModels = numModels; 
  init();
}

GLvoid Scene::init() {
  COLOR_CHANGE = 0.05f;

  m_vShader = -1;
  m_fShader = -1;
  m_shaderProgram = -1;
 
  m_cam      = new Camera();
  
  m_keyboard = new Keyboard();

  m_models   = new Model[m_numModels];
  m_modChoice= 0;
  
  m_color    = new Colorizer();
  
  m_ccw = GL_TRUE;
  m_backfaceCulling = GL_TRUE;
}

Scene::~Scene() {
  delete m_cam;
  delete m_keyboard;
  delete [] m_models;
  delete m_color;
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

GLvoid Scene::loadDefaultShaders() {
  m_vShader = glCreateShader( GL_VERTEX_SHADER );
  m_fShader = glCreateShader( GL_FRAGMENT_SHADER );

  char* vs = textFileRead( "../shaders/vertex/vertexshader.txt" );
  char* fs = textFileRead( "../shaders/fragment/fragmentshader.txt" );

  const GLchar * vss = vs;
  const GLchar * fss = fs;

  glShaderSource( m_vShader, 1, &vss, NULL );
  glShaderSource( m_fShader, 1, &fss, NULL );
  free( vs );
  free( fs );

  glCompileShader( m_vShader );
  glCompileShader( m_fShader );

  printShaderInfoLog( m_vShader );
  printShaderInfoLog( m_fShader );

  m_shaderProgram = glCreateProgram();
  glAttachShader( m_shaderProgram, m_vShader );
  glAttachShader( m_shaderProgram, m_fShader );
  
  glLinkProgram( m_shaderProgram );
  printProgramInfoLog( m_shaderProgram );

  glUseProgram( m_shaderProgram );
}

GLint Scene::loadShader( char* loc, GLenum type ) {
  GLuint shaderID = glCreateShader( type );
  char* shaderSource = textFileRead( loc );
  const GLchar * shaderSource2D = shaderSource;

  glShaderSource( shaderID, 1, &shaderSource2D, NULL );
  free( shaderSource );

  glCompileShader( shaderID );
  printShaderInfoLog( shaderID );

  GLuint shaderProgram = glCreateProgram();
  glAttachShader( shaderProgram, shaderID );
  glLinkProgram( shaderProgram );
  printProgramInfoLog( shaderProgram );

  glUseProgram( shaderProgram );

  return shaderProgram;
}

GLvoid Scene::printShaderInfoLog( GLuint shaderID )
{
  GLint infoLogLength = 0;
  GLint count = 0;
  char* infoLog;

  glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
  
  if ( infoLogLength > 0 ) {
    infoLog = (GLchar*)malloc(infoLogLength);
    glGetShaderInfoLog( shaderID, infoLogLength, &count, infoLog );
    printf( "%s\n", infoLog );
    free( infoLog );
  }
}

GLvoid Scene::printProgramInfoLog( GLuint programID )
{
  GLint infoLogLength = 0;
  GLint count = 0;
  char* infoLog;

  glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );

  if ( infoLogLength > 0 ) { 
    infoLog = (GLchar*)malloc(infoLogLength);
    glGetProgramInfoLog( programID, infoLogLength, &count, infoLog );
    printf( "%s\n", infoLog );
    free( infoLog );
  }
}

GLvoid Scene::print() {
  cout << "Window = " << windowWidth << "x" << windowHeight << endl;
  cout << "Model choice = " << m_modChoice << endl;
  cout << "Render mode = " << (GLint)m_rMode << endl;
  cout << "CCW drawing = " << (GLint)m_ccw << endl;
  cout << "Backface culling = " << (GLint)m_backfaceCulling << endl;
  cout << "Shader Program ID = " << m_shaderProgram << endl;
  cout << "Vertex Shader ID = " << m_vShader << endl;
  cout << "Fragment Shader ID = " << m_fShader << endl;

  m_cam->print();
  //m_keyboard->print();
  m_color->print();
}

GLvoid Scene::drawCurrentModel() {

  // Set the color to draw. glVertexAttrib4fv cannot be called between a 
  // glBegin() and glEnd() block, so set it before!
  GLint vColor = glGetAttribLocation( m_shaderProgram, "vColor" );
  glVertexAttrib4fv( vColor, m_color->m_color );

  // Set modelView matrix
  GLint modelView = glGetUniformLocation( m_shaderProgram, "myModelViewMatrix" );
  glUniformMatrix4fv( modelView, 1, GL_FALSE, m_cam->m_modelViewMatr );

  // Set projection matrix
  GLint projection = glGetUniformLocation( m_shaderProgram, "myProjectionMatrix" );
  glUniformMatrix4fv( projection, 1, GL_FALSE, m_cam->m_projMatr );

  cout << "modelView = " << modelView << endl;
  cout << "projection = " << projection << endl;

  glFrontFace( m_ccw ? GL_CCW : GL_CW );    

  if ( m_backfaceCulling == GL_TRUE ) { glEnable( GL_CULL_FACE ); }
  else { glDisable( GL_CULL_FACE ); }

  if ( m_rMode == R_POINT ) {
    glPolygonMode( GL_FRONT_AND_BACK, GL_POINTS );
    glBegin( GL_POINTS );
  }
  
  else if ( m_rMode == R_WIREFRAME ) { 
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin( GL_TRIANGLES );
  }

  else if ( m_rMode == R_SOLID ) { 
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin( GL_TRIANGLES );
  }

  for ( GLuint i = 0; i < m_models[m_modChoice].m_tris.size(); i++ ) {
    for ( GLuint j = 0; j < 3; j++ ) {
      // TODO use arraylist/displaylist somewhere 
      glVertex3fv( m_models[m_modChoice].m_tris[i].m_verts[j] );
    }
  }

  glEnd();
}

GLvoid Scene::centerOnCurrentModel() {
  m_cam->centerOn( m_models[m_modChoice] );
}

GLvoid Scene::handleKeys() {
  if ( m_keyboard->isPressed(27) ) { exit(0); }

  // Reset view
  if ( m_keyboard->isPressed('r') || m_keyboard->isPressed('R') ) { centerOnCurrentModel(); }
  // Translation along specific axes by 5% of model size
  if ( m_keyboard->isPressed('a') || m_keyboard->isPressed('A') ) { m_cam->translate( 'u', 1 ); }
  if ( m_keyboard->isPressed('d') || m_keyboard->isPressed('D') ) { m_cam->translate( 'u', -1 ); }
  if ( m_keyboard->isPressed('w') || m_keyboard->isPressed('W') ) { m_cam->translate( 'n', 1 ); }
  if ( m_keyboard->isPressed('s') || m_keyboard->isPressed('S') ) { m_cam->translate( 'n', -1 ); }
  if ( m_keyboard->isPressed('q') || m_keyboard->isPressed('Q') ) { m_cam->translate( 'v', 1 ); }
  if ( m_keyboard->isPressed('e') || m_keyboard->isPressed('E') ) { m_cam->translate( 'v', -1 ); }
  if ( m_keyboard->isPressed('z') || m_keyboard->isPressed('Z') ) { m_cam->translate(5.0, 5.0, 5.0); }
  // Rotation about specific axes by 10 degrees
  if ( m_keyboard->isPressed('l') || m_keyboard->isPressed('L') ) { m_cam->rotate('v', 1 ); }
  if ( m_keyboard->isPressed('\'') )                   { m_cam->rotate('v', -1 ); }
  if ( m_keyboard->isPressed('p') || m_keyboard->isPressed('P') ) { m_cam->rotate('u', 1 );  }
  if ( m_keyboard->isPressed(';') )                    { m_cam->rotate('u', -1 ); }
  if ( m_keyboard->isPressed('o') || m_keyboard->isPressed('O') ) { m_cam->rotate('n', 1 ); }
  if ( m_keyboard->isPressed('[') )                    { m_cam->rotate('n', -1 ); }
  // Change drawing colors
  if ( m_keyboard->isPressed('1') )                    { m_color->changeColor( 'r', 1 ); }
  if ( m_keyboard->isPressed('2') )                    { m_color->changeColor( 'g', 1 ); }
  if ( m_keyboard->isPressed('3') )                    { m_color->changeColor( 'b', 1 ); }
  if ( m_keyboard->isPressed('4') )                    { m_color->changeColor( 'a', 1 ); }
  if ( m_keyboard->isPressed('5') )                    { m_color->changeColor( 'r', -1 ); }
  if ( m_keyboard->isPressed('6') )                    { m_color->changeColor( 'g', -1 ); }
  if ( m_keyboard->isPressed('7') )                    { m_color->changeColor( 'b', -1 ); }
  if ( m_keyboard->isPressed('8') )                    { m_color->changeColor( 'a', -1 ); }
  if ( m_keyboard->isPressed('9') )                    { m_cam->moveClippingPlane( 'f', 1 ); }
  if ( m_keyboard->isPressed('0') )                    { m_cam->moveClippingPlane( 'n', 1 ); }
  if ( m_keyboard->isPressed('-') )                    { m_cam->moveClippingPlane( 'f', -1 ); }
  if ( m_keyboard->isPressed('=') )                    { m_cam->moveClippingPlane( 'n', -1 ); }
  if ( m_keyboard->isPressed(' ') ) {
    m_modChoice = (m_modChoice + 1) % m_numModels;
    centerOnCurrentModel();
  }
}
