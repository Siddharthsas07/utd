#include "HandModel.h"

HandModel::HandModel() {}

HandModel::HandModel( const HandModel& rhs ) {
  model = rhs.model;
  params = rhs.params;
}

HandModel::HandModel( const vector<float>& values ) {
  BoundedParameter<float> par;
  par.l = -FLT_MAX;
  par.u = FLT_MAX;

  for ( unsigned int i = 0; i < values.size(); i++ ) {
    par.v = values[i];
    params.push_back( par );
  }

  update_model();
}

HandModel::~HandModel() {}

HandModel& HandModel::operator=( const HandModel& rhs ) {
  if ( this == &rhs ) {
    return *this;
  }

  params = rhs.params;
  model = rhs.model;

  return *this;
}

// update model's values using values from params. param values are updated from the PSOer each frame
// so keep the model tree up to date too, so it can be rendered later
HRESULT HandModel::update_model() {
  HRESULT hr;

  return hr;
}

vector<float> HandModel::get_params_without_bounds() {
  vector<float> res;
  for ( unsigned int i = 0; i < params.size(); i++ ) {
    res.push_back( params[i].v );
  }
  return res;
}