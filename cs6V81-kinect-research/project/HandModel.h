#pragma once

#include <Windows.h>
#include <vector>
#include "ModelUtils.h"
#include "useful_things.h"

using std::vector;

class HandModel {
  public:
    HandModel();
    HandModel( const HandModel& rhs );
    HandModel( const vector<float>& values );
    ~HandModel();
    HandModel& operator=( const HandModel& rhs );

    vector<BoundedParameter<float> > params;


    ModelTree model;

    HRESULT update_model();
    vector<float> get_params_without_bounds();
};
