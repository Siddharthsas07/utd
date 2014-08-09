#ifndef QUATERNION_H
#define QUATERNION_H

#include <Windows.h>
#include <xnamath.h>

class Quaternion {
  public:
    XMFLOAT4 values;

    Quaternion();
    Quaternion( float x, float y, float z, float w );
    Quaternion( const Quaternion& q );
    ~Quaternion();

    Quaternion& operator=( const Quaternion& rhs );

    XMMATRIX to_matrix();
    HRESULT from_matrix( const XMMATRIX& mat );
};

#endif