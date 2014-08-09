#pragma once

#include "Quaternion.h"
#include <xnamath.h>

class Triangle {
  public:
    Triangle();
    Triangle( float a, float b, float c );
    ~Triangle();

    Triangle& operator=( const Triangle& rhs );

    XMFLOAT3 values;
};

class Geometry {
  public:
    Geometry();
    Geometry( const Geometry& rhs );
    ~Geometry();    
    Geometry& operator=( const Geometry& rhs );

    Triangle* tris;
    XMFLOAT4* norms;

    size_t   num_tris;
};

class ModelNode {
  public:
    ModelNode();
    ModelNode( const ModelNode& rhs );
    ~ModelNode();
    ModelNode& operator=( const ModelNode& rhs );

    XMFLOAT3    trans; // this translation
    Quaternion  rot;   // this orientation
    Geometry    geo;// this geometry (cylinder, etc.)
    ModelNode*  p;  // parent
    ModelNode** c;  // children
    size_t      nc; // number children

    HRESULT add_child( ModelNode* c );
    HRESULT set_parent( ModelNode* p );
};

class ModelTree {
  private:
    ModelNode* itr;

  public:
    ModelTree();
    ModelTree( const ModelTree& rhs );
    ~ModelTree();
    ModelTree& operator=( const ModelTree& rhs );

    ModelNode* root; // root of model tree

    ModelNode* next_df_traversal();
    void       reset_traversal();
};

