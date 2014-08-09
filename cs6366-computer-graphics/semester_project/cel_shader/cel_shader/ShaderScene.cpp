#include "ShaderScene.h"

using namespace std;

ShaderScene::ShaderScene() {
  init();
}

ShaderScene::ShaderScene( GLuint numModels ) {
  m_models.resize( numModels );
  init();
}

GLvoid ShaderScene::init() {
  COLOR_CHANGE = 0.05f;

  globalAmbientLoc = -1;
  lightAmbientLoc = -1;
  lightDiffuseLoc = -1;
  lightSpecularLoc = -1;
  matAmbientLoc = -1;
  matDiffuseLoc = -1;
  matSpecularLoc = -1;
  matShininessLoc = -1;
  lightLocationLoc = -1;
  vertexNormalLoc = -1;
  vertexPositionLoc = -1;
  lightsEnabledLoc = -1;
  modelViewLocationLoc = -1;
  projectionLocationLoc = -1;
  numLevelsLoc = -1;
  intervalPercentLoc = -1;
  
  m_windowWidth = WINDOW_WIDTH;
  m_windowHeight = WINDOW_HEIGHT;
  m_windowID = -1;
 
  m_cam      = new ShaderCamera();
  
  m_keyboard = new Keyboard();

  m_modChoice= 0;
  
  m_color    = new Colorizer();
  
  m_lightDelta = 0.05f;
  m_lightsEnabled = GL_FALSE;
  m_smoothShadingEnabled = GL_FALSE;
  m_silhouetteEnabled = GL_TRUE;
  
  m_ccw = GL_TRUE;
  m_backfaceCulling = GL_TRUE;
}

ShaderScene::~ShaderScene() {
  delete m_cam;
  delete m_keyboard;
  delete m_color;
}

GLvoid ShaderScene::loadDefaultShaders() {

  m_shaderProgram.read( "../shaders/phong_cel.vert", "../shaders/phong_cel.frag" );
  m_shaderProgram.compile();
  m_shaderProgram.m_vert.printLog();
  m_shaderProgram.m_frag.printLog();
  m_shaderProgram.printLog();  
 
  globalAmbientLoc = glGetAttribLocation( m_shaderProgram.m_ID, "globalAmbient" );
  lightAmbientLoc = glGetAttribLocation( m_shaderProgram.m_ID, "lightAmbient" );
  lightDiffuseLoc = glGetAttribLocation( m_shaderProgram.m_ID, "lightDiffuse" );
  lightSpecularLoc = glGetAttribLocation( m_shaderProgram.m_ID, "lightSpecular" );
  matAmbientLoc = glGetAttribLocation( m_shaderProgram.m_ID, "matAmbient" );
  matDiffuseLoc = glGetAttribLocation( m_shaderProgram.m_ID, "matDiffuse" );
  matSpecularLoc = glGetAttribLocation( m_shaderProgram.m_ID, "matSpecular" );
  matShininessLoc = glGetAttribLocation( m_shaderProgram.m_ID, "matShininess" );
  lightLocationLoc = glGetAttribLocation( m_shaderProgram.m_ID, "lightLocation" );
  viewerLocationLoc = glGetAttribLocation( m_shaderProgram.m_ID, "viewerLocation" );
  vertexNormalLoc = glGetAttribLocation( m_shaderProgram.m_ID, "vertexNormal" );
  vertexPositionLoc = glGetAttribLocation( m_shaderProgram.m_ID, "vertexPosition" );
  lightsEnabledLoc = glGetUniformLocation( m_shaderProgram.m_ID, "lightsEnabled" );
  modelViewLocationLoc = glGetUniformLocation( m_shaderProgram.m_ID, "modelViewMatr" );
  projectionLocationLoc = glGetUniformLocation( m_shaderProgram.m_ID, "projMatr" );
  numLevelsLoc = glGetUniformLocation( m_shaderProgram.m_ID, "numLevels" );
  intervalPercentLoc = glGetUniformLocation( m_shaderProgram.m_ID, "intervalPercent" );
}

GLvoid ShaderScene::print() {
  cout << "Window = " << m_windowWidth << "x" << m_windowHeight << endl;
  cout << "Model choice = " << m_modChoice << endl;
  cout << "Render mode = " << (GLint)m_rMode << endl;
  cout << "CCW drawing = " << (GLint)m_ccw << endl;
  cout << "Backface culling = " << (GLint)m_backfaceCulling << endl;
  cout << "Shader Program ID = " << m_shaderProgram.m_ID << endl;
  cout << "Vertex Shader ID = " << m_shaderProgram.m_vert.m_ID << endl;
  cout << "Fragment Shader ID = " << m_shaderProgram.m_frag.m_ID << endl;
  cout << "globalAmbientLoc = " << globalAmbientLoc << endl;
  cout << "lightAmbientLoc = " << lightAmbientLoc << endl;
  cout << "lightDiffuseLoc = " << lightDiffuseLoc << endl;
  cout << "lightSpecularLoc = " << lightSpecularLoc << endl;
  cout << "matAmbientLoc = " << matAmbientLoc << endl;
  cout << "matDiffuseLoc = " << matDiffuseLoc << endl;
  cout << "matSpecularLoc = " << matSpecularLoc << endl;
  cout << "matShininessLoc = " << matShininessLoc << endl;
  cout << "lightLocationLoc = " << lightLocationLoc << endl;
  cout << "viewerLocationLoc = " << viewerLocationLoc << endl;
  cout << "vertexNormalLoc = " << vertexNormalLoc << endl;
  cout << "vertexPositionLoc = " << vertexPositionLoc << endl;
  cout << "lightsEnabledLoc = " << lightsEnabledLoc <<  endl;
  cout << "modelViewLocationLoc = " << modelViewLocationLoc << endl;
  cout << "projectionLocationLoc = " << projectionLocationLoc << endl;
  cout << "numLevelsLoc = " << numLevelsLoc << endl;
  cout << "intervalPercentLoc = " << intervalPercentLoc << endl;
  cout << "Global Ambient = "; m_globalAmbient.print();
  for ( GLuint i = 0; i < m_lights.size(); i++ ) {
    m_lights[i]->print();
  }
  m_cam->print();
  m_color->print();
}

GLvoid ShaderScene::setDefaultLights() {
  m_lightsEnabled = GL_TRUE;
  m_smoothShadingEnabled = GL_TRUE;
  m_silhouetteEnabled = GL_TRUE;
  
  m_globalAmbient.setAmbient( 0.1f, 0.1f, 0.1f, 0.1f );

  Light * light = new Light();
  light->setAmbient( 0.2f, 0.2f, 0.2f, 1.0f );
  light->setDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
  light->setSpecular( 0.1f, 0.1f, 0.1f, 1.0f );
  light->setPosition( 0.0f, 0.0f, 0.0f );
  m_lights.push_back( light );
  glEnable( GL_DEPTH_TEST );
}

GLvoid ShaderScene::pairwiseMultiply( col4& res, const col4& lhs, const GLfloat* rhs ) {
  res[0] = lhs[0] * rhs[0];
  res[1] = lhs[1] * rhs[1];
  res[2] = lhs[2] * rhs[2];
  res[3] = lhs[3] * rhs[3];
}

GLvoid ShaderScene::drawCurrentModel() {
  if ( m_shaderProgram.m_ID == -1 ) {
    cout << "Shaders have not been loaded. Cannot draw." << endl;
  }

  // Set modelView matrix
  glUniformMatrix4fv( modelViewLocationLoc, 1, GL_FALSE, m_cam->m_modelViewMatr );

  // Set projection matrix
  glUniformMatrix4fv( projectionLocationLoc, 1, GL_FALSE, m_cam->m_projMatr );

  // Set the number of shading levels and also the percent for each level
  glUniform1i( numLevelsLoc, 4 );
  glUniform1f( intervalPercentLoc, 0.4 );

  // Tell the shader lights are on or off
  glUniform1f( lightsEnabledLoc, (m_lightsEnabled ? 1.0f : 0.0f) );

  glShadeModel( m_smoothShadingEnabled ? GL_SMOOTH : GL_FLAT );
  glFrontFace( m_ccw ? GL_CCW : GL_CW );
  glEnable( GL_CULL_FACE );
  glCullFace( GL_BACK );

  GLint matIndex = 0;
  GLuint numTris = 0;

  // Lighting is turned on
  if ( m_lightsEnabled ) {
    glBegin( GL_TRIANGLES );
    numTris = m_models[m_modChoice].m_tris.size();   

    for ( GLuint i = 0; i < numTris; i++ ) {
      // Specify material triangle is made of, assume all 3 vertices are made of same material
      matIndex = m_models[m_modChoice].m_tris[i].m_matIdx[0];
      glVertexAttrib4fv( globalAmbientLoc, m_globalAmbient.m_amb );

      glVertexAttrib4fv( lightAmbientLoc, m_lights[0]->m_amb );
      glVertexAttrib4fv( lightDiffuseLoc, m_lights[0]->m_dif );
      glVertexAttrib4fv( lightSpecularLoc, m_lights[0]->m_spec );

      glVertexAttrib4fv( matAmbientLoc, m_models[m_modChoice].m_mats[matIndex].m_ambient );
      glVertexAttrib4fv( matDiffuseLoc, m_models[m_modChoice].m_mats[matIndex].m_diffuse );
      glVertexAttrib4fv( matSpecularLoc,m_models[m_modChoice].m_mats[matIndex].m_specular );
      glVertexAttrib1f( matShininessLoc, m_models[m_modChoice].m_mats[matIndex].m_shininess );

      glVertexAttrib3fv( lightLocationLoc, m_lights[0]->m_pos );
      glVertexAttrib3fv( viewerLocationLoc, m_cam->m_pos );

      // Specify properties for the current vertex.
      for ( GLuint j = 0; j < 3; j++ ) {        
        glVertexAttrib3fv( vertexNormalLoc, m_models[m_modChoice].m_tris[i].m_norms[j] );
        glVertexAttrib3fv( vertexPositionLoc, m_models[m_modChoice].m_tris[i].m_verts[j] );
      }
    }

    glEnd();
  }

  // Lighting is turned off
  else {
    glBegin( GL_TRIANGLES );

    GLuint numTris = m_models[m_modChoice].m_tris.size();
    for ( GLuint i = 0; i < numTris; i++ ) {
      // Specify material triangle is made of, assume all 3 vertices are made of same material
      matIndex = m_models[m_modChoice].m_tris[i].m_matIdx[0];
      glVertexAttrib4fv( matDiffuseLoc, m_models[m_modChoice].m_mats[matIndex].m_diffuse  );

      // Specify vertices for triangle
      for ( GLuint j = 0; j < 3; j++ ) {        
        glVertexAttrib3fv( vertexPositionLoc, m_models[m_modChoice].m_tris[i].m_verts[j] );
        glVertexAttrib3fv( vertexNormalLoc, m_models[m_modChoice].m_tris[i].m_norms[j] );
      }
    }

    glEnd();
  }
}

GLvoid ShaderScene::centerOnCurrentModel() {
  m_cam->centerOn( m_models[m_modChoice], m_windowWidth, m_windowHeight );
}

GLvoid ShaderScene::handleKeys() {
  if ( m_keyboard->isPressed(27) ) { exit(0); }
  // Alter global ambient light colors
  if ( m_keyboard->isPressed('1') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Global Red Increase" << endl; 
    m_globalAmbient.m_amb[0] += m_lightDelta;
    clamp( m_globalAmbient.m_amb[0] );
  }
  if ( m_keyboard->isPressed('1') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Global Red Decrease" << endl; 
    m_globalAmbient.m_amb[0] -= m_lightDelta;
    clamp( m_globalAmbient.m_amb[0] );
  }
  if ( m_keyboard->isPressed('2') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Green Increase" << endl;
   m_globalAmbient.m_amb[1] += m_lightDelta;
    clamp( m_globalAmbient.m_amb[1] );
  }
  if ( m_keyboard->isPressed('2') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Green Decrease" << endl; 
    m_globalAmbient.m_amb[1] -= m_lightDelta;
    clamp( m_globalAmbient.m_amb[1] );
  }
  if ( m_keyboard->isPressed('3') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Blue Increase" << endl; 
    m_globalAmbient.m_amb[2] += m_lightDelta;
    clamp( m_globalAmbient.m_amb[2] );
  }
  if ( m_keyboard->isPressed('3') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Blue Decrease" << endl; 
    m_globalAmbient.m_amb[2] -= m_lightDelta;
    clamp( m_globalAmbient.m_amb[2] );
  }
  if ( m_keyboard->isPressed('4') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Global Alpha Increase" << endl; 
    m_globalAmbient.m_amb[3] += m_lightDelta;
    clamp( m_globalAmbient.m_amb[3] );
  }
  if ( m_keyboard->isPressed('4') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Global Alpha Decrease" << endl; 
    m_globalAmbient.m_amb[3] -= m_lightDelta;
    clamp( m_globalAmbient.m_amb[3] );
  }
  // Alter light source colors. ASSUMES AT LEAST ONE LIGHT HAS ENTERED THE SCENE.
  if ( (m_keyboard->isPressed('z') || m_keyboard->isPressed('Z')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Red Increase" << endl; 
    m_lights[0]->m_amb[0] += m_lightDelta;
    clamp( m_lights[0]->m_amb[0] );
  }
  if ( (m_keyboard->isPressed('z') || m_keyboard->isPressed('Z')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Red Decrease" << endl; 
    m_lights[0]->m_amb[0] -= m_lightDelta;
    clamp( m_lights[0]->m_amb[0] );
  }
  if ( (m_keyboard->isPressed('x') || m_keyboard->isPressed('X')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Green Increase" << endl; 
    m_lights[0]->m_amb[1] += m_lightDelta;
    clamp( m_lights[0]->m_amb[1] );
  }
  if ( (m_keyboard->isPressed('x') || m_keyboard->isPressed('X')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Green Decrease" << endl; 
    m_lights[0]->m_amb[1] -= m_lightDelta;
    clamp( m_lights[0]->m_amb[1] );
  }
  if ( (m_keyboard->isPressed('c') || m_keyboard->isPressed('C')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Blue Increase" << endl; 
    m_lights[0]->m_amb[2] += m_lightDelta;
    clamp( m_lights[0]->m_amb[2] );
  }
  if ( (m_keyboard->isPressed('c') || m_keyboard->isPressed('C')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Blue Decrease" << endl; 
    m_lights[0]->m_amb[2] -= m_lightDelta;
    clamp( m_lights[0]->m_amb[2] );
  }
  if ( (m_keyboard->isPressed('v') || m_keyboard->isPressed('V')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Ambient Alpha Increase" << endl;
    m_lights[0]->m_amb[3] += m_lightDelta;
    clamp( m_lights[0]->m_amb[3] );
  }
  if ( (m_keyboard->isPressed('v') || m_keyboard->isPressed('V')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Ambient Alpha Decrease" << endl; 
    m_lights[0]->m_amb[3] -= m_lightDelta;
    clamp( m_lights[0]->m_amb[3] );
  }
  if ( m_keyboard->isPressed('5') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Local Diffuse Red Increase" << endl; 
    m_lights[0]->m_dif[0] += m_lightDelta;
    clamp( m_lights[0]->m_dif[0] );
  }
  if ( m_keyboard->isPressed('5') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Local Diffuse Red Decrease" << endl; 
    m_lights[0]->m_dif[0] -= m_lightDelta;
    clamp( m_lights[0]->m_dif[0] );
  }
  if ( m_keyboard->isPressed('6') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) { 
    cout << "Local Diffuse Green Increase" << endl; 
    m_lights[0]->m_dif[1] += m_lightDelta;
    clamp( m_lights[0]->m_dif[1] );
  }
  if ( m_keyboard->isPressed('6') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) { 
    cout << "Local Diffuse Green Decrease" << endl; 
    m_lights[0]->m_dif[1] -= m_lightDelta;
    clamp( m_lights[0]->m_dif[1] );
  }
  if ( m_keyboard->isPressed('7') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Diffuse Blue Increase" << endl;
    m_lights[0]->m_dif[2] += m_lightDelta;
    clamp( m_lights[0]->m_dif[2] );
  }
  if ( m_keyboard->isPressed('7') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Diffuse Blue Decrease" << endl; 
    m_lights[0]->m_dif[2] -= m_lightDelta;
    clamp( m_lights[0]->m_dif[2] );
  }
  if ( m_keyboard->isPressed('8') && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Diffuse Alpha Increase" << endl;
    m_lights[0]->m_dif[3] += m_lightDelta;
    clamp( m_lights[0]->m_dif[3] );
  }
  if ( m_keyboard->isPressed('8') && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Diffuse Alpha Decrease" << endl; 
    m_lights[0]->m_dif[3] -= m_lightDelta;
    clamp( m_lights[0]->m_dif[3] );
  }
  if ( (m_keyboard->isPressed('b') || m_keyboard->isPressed('B')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Red Increase" << endl; 
    m_lights[0]->m_spec[0] += m_lightDelta;
    clamp( m_lights[0]->m_spec[0] );
  }
  if ( (m_keyboard->isPressed('b') || m_keyboard->isPressed('B')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Red Decrease" << endl; 
    m_lights[0]->m_spec[0] -= m_lightDelta;
    clamp( m_lights[0]->m_spec[0] );
  }
  if ( (m_keyboard->isPressed('n') || m_keyboard->isPressed('N')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Green Increase" << endl; 
    m_lights[0]->m_spec[1] += m_lightDelta;
    clamp( m_lights[0]->m_spec[1] );
  }
  if ( (m_keyboard->isPressed('n') || m_keyboard->isPressed('N')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Green Decrease" << endl; 
    m_lights[0]->m_spec[1] -= m_lightDelta;
    clamp( m_lights[0]->m_spec[1] );
  }
  if ( (m_keyboard->isPressed('m') || m_keyboard->isPressed('M')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Blue Increase" << endl; 
    m_lights[0]->m_spec[2] += m_lightDelta;
    clamp( m_lights[0]->m_spec[2] );
  }
  if ( (m_keyboard->isPressed('m') || m_keyboard->isPressed('M')) 
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Blue Decrease" << endl; 
    m_lights[0]->m_spec[2] -= m_lightDelta;
    clamp( m_lights[0]->m_spec[2] );
  }
  if ( m_keyboard->isPressed(',')
    && m_keyboard->isSpecialPressed(GLUT_KEY_UP) ) {
    cout << "Local Specular Alpha Increase" << endl;
    m_lights[0]->m_spec[3] += m_lightDelta;
    clamp( m_lights[0]->m_spec[3] );
  }
  if ( m_keyboard->isPressed(',')
    && m_keyboard->isSpecialPressed(GLUT_KEY_DOWN) ) {
    cout << "Local Specular Alpha Decrease" << endl; 
    m_lights[0]->m_spec[3] -= m_lightDelta;
    clamp( m_lights[0]->m_spec[3] );
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
  if ( m_keyboard->isPressed('u') || m_keyboard->isPressed('U') ) { m_smoothShadingEnabled = !m_smoothShadingEnabled; }
  if ( m_keyboard->isPressed('j') || m_keyboard->isPressed('J') ) { m_silhouetteEnabled = !m_silhouetteEnabled; }
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
    m_modChoice = (m_modChoice + 1) % m_models.size();
    centerOnCurrentModel();
  }
}
