#include "Camera.h"

using namespace std;

GLvoid Camera::calcTranslateDelta( const Model& m ) {
  // Move 5% of model width/etc for each translation
  m_translateDelta[0] = (m.m_box.m_max[0] - m.m_box.m_min[0]) * DEFAULT_TRANSLATE_STEP;
  m_translateDelta[1] = (m.m_box.m_max[1] - m.m_box.m_min[1]) * DEFAULT_TRANSLATE_STEP;
  m_translateDelta[2] = (m.m_box.m_max[2] - m.m_box.m_min[2]) * DEFAULT_TRANSLATE_STEP;
}

GLvoid Camera::calcRotateDelta( const Model& m ) {
  // 50 rotations to complete a circle
  m_rotateDelta[0] = DEFAULT_ROTATE_ANGLE;
  m_rotateDelta[1] = DEFAULT_ROTATE_ANGLE;
  m_rotateDelta[2] = DEFAULT_ROTATE_ANGLE;
}
