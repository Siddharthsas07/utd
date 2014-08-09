#include "Camera.h"

using namespace std;

GLvoid Camera::calcTranslateDelta( const Model& m ) {
  // Move 5% of model width/etc for each translation
  m_translateDelta[0] = (m.m_box[3] - m.m_box[0]) * .05f;
  m_translateDelta[1] = (m.m_box[4] - m.m_box[1]) * .05f;
  m_translateDelta[2] = (m.m_box[5] - m.m_box[2]) * .05f;
}

GLvoid Camera::calcRotateDelta( const Model& m ) {
  // 50 rotations to complete a circle
  m_rotateDelta[0] = 360.0f / 50.0f;
  m_rotateDelta[1] = 360.0f / 50.0f;
  m_rotateDelta[2] = 360.0f / 50.0f;
}
