#include "Quaternion.h"

Quaternion::Quaternion() : values(0.0, 0.0, 0.0, 0.0) {
}

Quaternion::Quaternion( float x, float y, float z, float w ) {
  values.x = x;
  values.y = y;
  values.z = z;
  values.w = w;
}

Quaternion::Quaternion( const Quaternion& rhs ) {
  values.x = rhs.values.x;
  values.y = rhs.values.y;
  values.z = rhs.values.z;
  values.w = rhs.values.w;
}

Quaternion::~Quaternion() {
  // nada
}

Quaternion& Quaternion::operator=( const Quaternion& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  values.x = rhs.values.x;
  values.y = rhs.values.y;
  values.z = rhs.values.z;
  values.w = rhs.values.w;

  return *this;
}

XMMATRIX Quaternion::to_matrix() {
  XMMATRIX temp;
  // do some crazy math here
  return temp;
}

HRESULT Quaternion::from_matrix( const XMMATRIX& mat ) {
  HRESULT hr;
  // do some crazy math here

  return hr;
}
