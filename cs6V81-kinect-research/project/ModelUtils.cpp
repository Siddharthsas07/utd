#include "ModelUtils.h"
#include "useful_things.h"

Triangle::Triangle() : values(0.0, 0.0, 0.0 ) {
}

Triangle::Triangle( float x, float y, float z ) {
  values.x = x;
  values.y = y;
  values.z = z;
}

Triangle::~Triangle() {
  // nada
}

Triangle& Triangle::operator=( const Triangle& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  values.x = rhs.values.x;
  values.y = rhs.values.y;
  values.z = rhs.values.z;

  return *this;
}

Geometry::Geometry() : tris(NULL), norms(NULL), num_tris(0) {
}

Geometry::Geometry( const Geometry& rhs ) {
  num_tris = rhs.num_tris;

  // copy tris

  // copy norms
}

Geometry::~Geometry() {
  SAFE_DELETE( tris );
  SAFE_DELETE( norms );
}

Geometry& Geometry::operator=( const Geometry& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  num_tris = rhs.num_tris;

  // copy tris
  // copy norms

  return *this;
}

ModelNode::ModelNode() : trans(0.0, 0.0, 0.0), 
                         rot(0.0, 0.0, 0.0, 1.0),
                         p(NULL),
                         c(NULL),
                         nc(0) {
}

ModelNode::ModelNode( const ModelNode& rhs ) {
  trans.x = rhs.trans.x;
  trans.y = rhs.trans.y;
  trans.z = rhs.trans.z;

  rot = rhs.rot;

  geo = rhs.geo;

  // copy parent pointer

  nc = rhs.nc;

  // copy child pointers
  
}

ModelNode::~ModelNode() {
  for ( size_t i = 0; i < nc; i++ ) {
    SAFE_DELETE( c[i] );
  }
  //SAFE_DELETE( p );  
}

ModelNode& ModelNode::operator=( const ModelNode& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  trans.x = rhs.trans.x;
  trans.y = rhs.trans.y;
  trans.z = rhs.trans.z;

  rot = rhs.rot;

  geo = rhs.geo;

  // copy parent pointer

  nc = rhs.nc;

  // copy child pointers
  return *this;
}

HRESULT ModelNode::add_child( ModelNode* c ) {
  HRESULT hr;

  return hr;
}

HRESULT ModelNode::set_parent( ModelNode* p ) {
  this->p = p;
  return S_OK;
}

ModelTree::ModelTree() : root(NULL), itr(NULL) {
}

ModelTree::ModelTree( const ModelTree& rhs ) {
  // deep copy?
}

ModelTree::~ModelTree() {
  SAFE_DELETE( root ); // delete root, which deletes its children..on and on to NULL at leaves
}

ModelTree& ModelTree::operator=( const ModelTree& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  // deep copy?

  return *this;
}

ModelNode* ModelTree::next_df_traversal() {
  // return leftmost child

  // if none exists, gob ack up to parent and return next child down

  // if none exists...continue...that...a DFS traversal you know.

  return itr;
}

void ModelTree::reset_traversal() {
  itr = root;
}
