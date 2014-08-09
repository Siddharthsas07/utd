// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file exception.h
/// @edit 22 Sept 2009
/// @brief The header file for a generic exception class
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @class Exception
/// @brief Contains general exception handling routines and exception
///   instantiation.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn Exception( const int& errorCode, const std::stright& errorMsg );
/// @brief The constructor for the Exception class.
/// @pre An exception is thrown.
/// @param const int& errorCode is the number of the error code.
/// @param const std::string& errorMsg is the string describing the error 
/// @post Creates an exception with the passed error number and message.
/// @return N/A
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn ~Exception();
/// @brief The destructor for the Exception class.
/// @pre An exception exists.
/// @post Destroys the exception object.
/// @return N/A
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn int get_errorCode() const;
/// @brief Returns the errorCode of the object
/// @pre An exception exists.
/// @post Returns the errorCode of the object
/// @return Returns the errorCode of the object
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string get_errorMsg() const;
/// @brief Returns the errorMessage of the object.
/// @pre An exception exists.
/// @post Returns the errorMessage of the object.
/// @return Returns the errorMessage of the object.
///////////////////////////////////////////////////////////////////////////////

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class Exception {
  public:
    Exception( const int& errorCode, const std::string& errorMsg );
    int code() const;
    std::string message() const;
  
  private:
    int         m_errorCode;
    std::string m_errorMsg;
};

#endif
// END OF FILE-----------------------------------------------------------------
