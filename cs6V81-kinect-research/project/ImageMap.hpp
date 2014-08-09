#include "useful_things.h"

template<class T>
ImageMap<T>::ImageMap() {
  data = NULL;
  width = 0;
  height = 0;
}

template<class T>
ImageMap<T>::ImageMap( size_t width, size_t height ) {
  data = new T[width*height];
  this->width = width;
  this->height = height;
}

template<class T>
ImageMap<T>::ImageMap( size_t sz ) {
  data = new T[sz];
  width = sz;
  height = 1;
}

template<class T>
ImageMap<T>::~ImageMap() {
  SAFE_DELETE_ARRAY( data );
}

template<class T>
ImageMap<T>& ImageMap<T>::operator=( const ImageMap<T>& rhs ) {
  SAFE_DELETE_ARRAY( data );

  data = new T[rhs.width*rhs.height];

  width = rhs.width;
  height = rhs.height;
}

template<class T>
HRESULT ImageMap<T>::set_size( size_t width, size_t height ) {
  SAFE_DELETE_ARRAY( data );

  data = new T[width*height];

  this->width = width;
  this->height = height;
}

template<class T>
HRESULT ImageMap<T>::set_size( size_t sz) {
  SAFE_DELETE_ARRAY( data );

  data = new T[sz];
  width = sz;
  height = 1;
}

// TODO
template<class T>
T ImageMap<T>::get( size_t row, size_t col ) {
  return static_cast<T>(0);
}

template<class T>
T ImageMap<T>::get( size_t idx ) {
  return idx < width ? data[idx] : static_cast<T>(0);
}

// TODO
template<class T>
HRESULT ImageMap<T>::set( size_t row, size_t col, T value ) {
  return S_OK;
}

template<class T>
HRESULT ImageMap<T>::set( size_t idx, T value ) {
  if ( idx < width ) {
    data[idx] = value;
    return S_OK;
  }

  return S_ERROR;
}

template<class T>
void ImageMap<T>::zero() {
  memset( data, 0, width*height*sizeof(T) );
}