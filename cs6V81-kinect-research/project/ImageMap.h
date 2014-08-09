#pragma once

#include <Windows.h>

template<class T>
class ImageMap {
  public:
    ImageMap();
    ImageMap( size_t width, size_t height );
    ImageMap( size_t sz);
    ~ImageMap();

    ImageMap<T>& operator=( const ImageMap<T>& rhs );

    HRESULT set_size( size_t width, size_t height );
    HRESULT set_size( size_t sz);

    T* data;
    size_t width;
    size_t height;  

    T get( size_t row, size_t col );
    T get( size_t idx );

    HRESULT set( size_t row, size_t col, T value );
    HRESULT set( size_t idx, T value );

    void zero();
};

#include "ImageMap.hpp"