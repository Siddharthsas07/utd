#pragma once

#include "HandModel.h"
#include "ImageMap.h"

class HandModelRenderer {
  public:
    HandModelRenderer();
    ~HandModelRenderer();

    ImageMap<unsigned __int16> depth_map;
    ImageMap<unsigned __int32> color_map;

    HRESULT render( const HandModel& h );
};