#include "GLScene.h"

using namespace std;

GLScene::GLScene() {
  m_numModels = 0; 
  init();
}

GLScene::GLScene( GLuint numModels ) {
  m_numModels = numModels; 
  init();
}

GLvoid GLScene::init() {
  COLOR_CHANGE = 0.05f;

  m_windowWidth = WINDOW_WIDTH;
  m_windowHeight = WINDOW_HEIGHT;
  m_windowID = -1;
 
  m_cam      = new GLCamera();
  
  m_keyboard = new Keyboard();

  m_models   = new Model[m_numModels];
  m_modChoice= 0;
  
  m_color    = new Colorizer();

  m_light    = new Light();
  memset( m_globalAmbient, 0, 4*sizeof(GLfloat) );
  m_lightDelta = 0.05f;
  m_lightsEnabled = GL_FALSE;
  m_smoothShading = GL_FALSE;
  
  m_ccw = GL_TRUE;
  m_backfaceCulling = GL_TRUE;
}

GLScene::~GLScene() {
  delete m_cam;
  delete m_keyboard;
  delete [] m_models;
  delete m_color;
  delete m_light;
}

GLvoid GLScene::print() {
  cout << "GLScene: " << endl;
  cout << "  Window = " << m_windowWidth << "x" << m_windowHeight << endl;
  cout << "  Model choice = " << m_modChoice << endl;
  cout << "  Render mode = " << (GLint)m_rMode << endl;
  cout << "  CCW drawing = " << (GLint)m_ccw << endl;
  cout << "  Backface culling = " << (GLint)m_backfaceCulling << endl;
  cout << "  Global Ambient = " << m_globalAmbient[0] << " " <<m_globalAmbient[1] << " " << m_globalAmbient[2] << " " << m_globalAmbient[3] << endl;
  m_light->print();
  m_cam->print();
  m_color->print();
}

GLvoid GLScene::setDefaultLights() {
  m_lightsEnabled = GL_TRUE;
  m_smoothShading = GL_TRUE;
  m_globalAmbient[0] = 0.1f; m_globalAmbient[1] = 0.1f; m_globalAmbient[2] = 0.1f; m_globalAmbient[3] = 1.0f;
  m_light->setAmbient( 0.2f, 0.2f, 0.2f, 1.0f );
  m_light->setDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
  m_light->setSpecular( 0.1f, 0.1f, 0.1f, 1.0f );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
    glLoadIdentity();
    m_light->setPosition( 0.0f, 0.0f, 0.0f, 1.0f );
    glLightfv( GL_LIGHT0, GL_POSITION, m_light->m_pos );
  glPopMatrix();
  glLightfv( GL_LIGHT0, GL_AMBIENT, m_light->m_amb );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, m_light->m_dif );
  glLightfv( GL_LIGHT0, GL_SPECULAR, m_light->m_spec );
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, m_globalAmbient );

  glEnable( GL_NORMALIZE );
  glEnable( GL_DEPTH_TEST );
}

GLvoid GLScene::drawCurrentModel() {
  glShadeModel( m_smoothShading ? GL_SMOOTH : GL_FLAT );
  glFrontFace( m_ccw ? GL_CCW : GL_CW );
  glEnable( GL_CULL_FACE );
  glCullFace( GL_BACK );

  // Lighting is turned on
  if ( m_lightsEnabled ) {
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, m_globalAmbient );
    glLightfv( GL_LIGHT0, GL_AMBIENT, m_light->m_amb );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, m_light->m_dif );
    glLightfv( GL_LIGHT0, GL_SPECULAR, m_light->m_spec );

    glBegin( GL_TRIANGLES );

    GLuint numTris = m_models[m_modChoice].m_tris.size();

    for ( GLuint i = 0; i < numTris; i++ ) {
      // Specify material triangle is made of, assume all 3 vertices are made of same material
      GLuint matIndex = m_models[m_modChoice].m_tris[i].m_colorIndex[0];
      glMaterialfv( GL_FRONT, GL_AMBIENT, m_models[m_modChoice].m_ambient[matIndex] );
      glMaterialfv( GL_FRONT, GL_DIFFUSE, m_models[m_modChoice].m_diffuse[matIndex] );
      glMaterialfv( GL_FRONT, GL_SPECULAR, m_models[m_modChoice].m_specular[matIndex] );
      glMaterialf( GL_FRONT, GL_SHININESS, m_models[m_modChoice].m_shine[matIndex] );

      // Specify vertices for triangle
      for ( GLuint j = 0; j < 3; j++ ) {
        // TODO use arraylist/displaylist somewhere 
        glNormal3fv( m_models[m_modChoice].m_tris[i].m_norms[j] );
        glVertex3fv( m_models[m_modChoice].m_tris[i].m_verts[j] );
      }
    }

    glEnd();    
  }

  // Lighting is turned off
  else {
    glDisable( GL_LIGHTING );
    glDisable( GL_LIGHT0 );
    glBegin( GL_TRIANGLES );

    GLuint numTris = m_models[m_modChoice].m_tris.size();
    for ( GLuint i = 0; i < numTris; i++ ) {
      // Specify material triangle is made of, assume all 3 vertices are made of same material
      GLuint matIndex = m_models[m_modChoice].m_tris[i].m_colorIndex[0];
      glColor3fv( m_models[m_modChoice].m_diffuse[matIndex] ); // Use material diffuse color for triangle

      // Specify vertices for triangle
      for ( GLuint j = 0; j < 3; j++ ) {        
        glNormal3fv( m_models[m_modChoice].m_tris[i].m_norms[j] );
        glVertex3fv( m_models[m_modChoice].m_tris[i].m_verts[j] );
      }
    }

    glEnd();
  }
}

GLvoid GLScene::centerOnCurrentModel() {
  m_cam->centerOn( m_models[m_modChoice], m_windowWidth, m_windowHeight );
}

GLvoid GLScene::handleKeys() {
  if ( m_keyboard->isPressed(27) ) { exit(0); }
  // Alter global ambient light colors
  if ( m_keyboard->isPressed('1') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Global Red Increase" << endl; 
    m_globalAmbient[0] += m_lightDelta;
    clamp( m_globalAmbient[0] );
  }
  if ( m_keyboard->isPressed('1') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Global Red Decrease" << endl; 
    m_globalAmbient[0] -= m_lightDelta;
    clamp( m_globalAmbient[0] );
  }
  if ( m_keyboard->isPressed('2') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Green Increase" << endl;
     m_globalAmbient[1] += m_lightDelta;
    clamp( m_globalAmbient[1] );
  }
  if ( m_keyboard->isPressed('2') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Green Decrease" << endl; 
    m_globalAmbient[1] -= m_lightDelta;
   clamp( m_globalAmbient[1] );
  }
  if ( m_keyboard->isPressed('3') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Blue Increase" << endl; 
    m_globalAmbient[2] += m_lightDelta;
    clamp( m_globalAmbient[2] );
  }
  if ( m_keyboard->isPressed('3') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Blue Decrease" << endl; 
    m_globalAmbient[2] -= m_lightDelta;
    clamp( m_globalAmbient[2] );
  }
  if ( m_keyboard->isPressed('4') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Alpha Increase" << endl; 
    m_globalAmbient[3] += m_lightDelta;
    clamp( m_globalAmbient[3] );
  }
  if ( m_keyboard->isPressed('4') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Alpha Decrease" << endl; 
    m_globalAmbient[3] -= m_lightDelta;
    clamp( m_globalAmbient[3] );
  }
  // Alter light source colors
  if ( (m_keyboard->isPressed('z') || m_keyboard->isPressed('Z')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Red Increase" << endl; 
    m_light->m_amb[0] += m_lightDelta;
    clamp( m_light->m_amb[0] );
  }
  if ( (m_keyboard->isPressed('z') || m_keyboard->isPressed('Z')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Red Decrease" << endl; 
    m_light->m_amb[0] -= m_lightDelta;
    clamp( m_light->m_amb[0] );
  }
  if ( (m_keyboard->isPressed('x') || m_keyboard->isPressed('X')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Green Increase" << endl; 
    m_light->m_amb[1] += m_lightDelta;
    clamp( m_light->m_amb[1] );
  }
  if ( (m_keyboard->isPressed('x') || m_keyboard->isPressed('X')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Green Decrease" << endl; 
    m_light->m_amb[1] -= m_lightDelta;
    clamp( m_light->m_amb[1] );
  }
  if ( (m_keyboard->isPressed('c') || m_keyboard->isPressed('C')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Blue Increase" << endl; 
    m_light->m_amb[2] += m_lightDelta;
    clamp( m_light->m_amb[2] );
  }
  if ( (m_keyboard->isPressed('c') || m_keyboard->isPressed('C')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Blue Decrease" << endl; 
    m_light->m_amb[2] -= m_lightDelta;
    clamp( m_light->m_amb[2] );
  }
  if ( (m_keyboard->isPressed('v') || m_keyboard->isPressed('V')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Alpha Increase" << endl;
    m_light->m_amb[3] += m_lightDelta;
    clamp( m_light->m_amb[3] );
  }
  if ( (m_keyboard->isPressed('v') || m_keyboard->isPressed('V')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Alpha Decrease" << endl; 
    m_light->m_amb[3] -= m_lightDelta;
    clamp( m_light->m_amb[3] );
  }
  if ( m_keyboard->isPressed('5') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Local Diffuse Red Increase" << endl; 
    m_light->m_dif[0] += m_lightDelta;
    clamp( m_light->m_dif[0] );
  }
  if ( m_keyboard->isPressed('5') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Local Diffuse Red Decrease" << endl; 
    m_light->m_dif[0] -= m_lightDelta;
    clamp( m_light->m_dif[0] );
  }
  if ( m_keyboard->isPressed('6') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Local Diffuse Green Increase" << endl; 
    m_light->m_dif[1] += m_lightDelta;
    clamp( m_light->m_dif[1] );
  }
  if ( m_keyboard->isPressed('6') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Local Diffuse Green Decrease" << endl; 
    m_light->m_dif[1] -= m_lightDelta;
    clamp( m_light->m_dif[1] );
  }
  if ( m_keyboard->isPressed('7') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Diffuse Blue Increase" << endl;
    m_light->m_dif[2] += m_lightDelta;
    clamp( m_light->m_dif[2] );
  }
  if ( m_keyboard->isPressed('7') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Diffuse Blue Decrease" << endl; 
    m_light->m_dif[2] -= m_lightDelta;
    clamp( m_light->m_dif[2] );
  }
  if ( m_keyboard->isPressed('8') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Diffuse Alpha Increase" << endl;
    m_light->m_dif[3] += m_lightDelta;
    clamp( m_light->m_dif[3] );
  }
  if ( m_keyboard->isPressed('8') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Diffuse Alpha Decrease" << endl; 
    m_light->m_dif[3] -= m_lightDelta;
    clamp( m_light->m_dif[3] );
  }
  if ( (m_keyboard->isPressed('b') || m_keyboard->isPressed('B')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Red Increase" << endl; 
    m_light->m_spec[0] += m_lightDelta;
    clamp( m_light->m_spec[0] );
  }
  if ( (m_keyboard->isPressed('b') || m_keyboard->isPressed('B')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Red Decrease" << endl; 
    m_light->m_spec[0] -= m_lightDelta;
    clamp( m_light->m_spec[0] );
  }
  if ( (m_keyboard->isPressed('n') || m_keyboard->isPressed('N')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Green Increase" << endl; 
    m_light->m_spec[1] += m_lightDelta;
    clamp( m_light->m_spec[1] );
  }
  if ( (m_keyboard->isPressed('n') || m_keyboard->isPressed('N')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Green Decrease" << endl; 
    m_light->m_spec[1] -= m_lightDelta;
    clamp( m_light->m_spec[1] );
  }
  if ( (m_keyboard->isPressed('m') || m_keyboard->isPressed('M')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Blue Increase" << endl; 
    m_light->m_spec[2] += m_lightDelta;
    clamp( m_light->m_spec[2] );
  }
  if ( (m_keyboard->isPressed('m') || m_keyboard->isPressed('M')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Blue Decrease" << endl; 
    m_light->m_spec[2] -= m_lightDelta;
    clamp( m_light->m_spec[2] );
  }
  if ( m_keyboard->isPressed(',')
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Alpha Increase" << endl;
    m_light->m_spec[3] += m_lightDelta;
    clamp( m_light->m_spec[3] );
  }
  if ( m_keyboard->isPressed(',')
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Alpha Decrease" << endl; 
    m_light->m_spec[3] -= m_lightDelta;
    clamp( m_light->m_spec[3] );
  }
  // Alter clipping plane distances
  if ( m_keyboard->isPressed('9') ) { m_cam->moveClippingPlane( 'f', 1 ); }
  if ( m_keyboard->isPressed('0') ) { m_cam->moveClippingPlane( 'n', 1 ); }
  if ( m_keyboard->isPressed('-') ) { m_cam->moveClippingPlane( 'f', -1 ); }
  if ( m_keyboard->isPressed('=') ) { m_cam->moveClippingPlane( 'n', -1 ); }
  // Reset position
  if ( m_keyboard->isPressed('r') || m_keyboard->isPressed('R') ) { centerOnCurrentModel(); }
  if ( m_keyboard->isPressed('t') || m_keyboard->isPressed('T') ) { setDefaultLights(); }
  if ( m_keyboard->isPressed('y') || m_keyboard->isPressed('Y') ) { m_lightsEnabled = !m_lightsEnabled; }
  if ( m_keyboard->isPressed('u') || m_keyboard->isPressed('U') ) { m_smoothShading = !m_smoothShading; }
  if ( m_keyboard->isPressed('i') || m_keyboard->isPressed('I') ) { m_ccw = !m_ccw; }
  // Translation along specific axes by 5% of model size
  if ( m_keyboard->isPressed('a') || m_keyboard->isPressed('A') ) { m_cam->translate( 'u', 1 ); }
  if ( m_keyboard->isPressed('d') || m_keyboard->isPressed('D') ) { m_cam->translate( 'u', -1 ); }
  if ( m_keyboard->isPressed('w') || m_keyboard->isPressed('W') ) { m_cam->translate( 'n', 1 ); }
  if ( m_keyboard->isPressed('s') || m_keyboard->isPressed('S') ) { m_cam->translate( 'n', -1 ); }
  if ( m_keyboard->isPressed('q') || m_keyboard->isPressed('Q') ) { m_cam->translate( 'v', 1 ); }
  if ( m_keyboard->isPressed('e') || m_keyboard->isPressed('E') ) { m_cam->translate( 'v', -1 ); }
  // Rotation about specific axes by 10 degrees
  if ( m_keyboard->isPressed('l') || m_keyboard->isPressed('L') ) { m_cam->rotate('v', 1 ); }
  if ( m_keyboard->isPressed('\'') ) { m_cam->rotate('v', -1 ); }
  if ( m_keyboard->isPressed('p') || m_keyboard->isPressed('P') ) { m_cam->rotate('u', 1 );  }
  if ( m_keyboard->isPressed(';') ) { m_cam->rotate('u', -1 ); }
  if ( m_keyboard->isPressed('o') || m_keyboard->isPressed('O') ) { m_cam->rotate('n', 1 ); }
  if ( m_keyboard->isPressed('[') ) { m_cam->rotate('n', -1 ); }
  // Cycle to next model
  if ( m_keyboard->isPressed(' ') ) {
    m_modChoice = (m_modChoice + 1) % m_numModels;
    centerOnCurrentModel();
  }
}
