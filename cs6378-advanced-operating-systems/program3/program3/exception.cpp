// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file exception.cpp
/// @edit 19 Sept 2009
/// @brief Contains the implementation for the exception class
///////////////////////////////////////////////////////////////////////////////

#include "exception.h"

Exception::Exception( const int& errorCode, const std::string& errorMsg ) {
  m_errorCode = errorCode;
  m_errorMsg = errorMsg;
}

int Exception::code() const {
  return m_errorCode;
}

std::string Exception::message() const {
  return m_errorMsg;
}

// END OF FILE-----------------------------------------------------------------
