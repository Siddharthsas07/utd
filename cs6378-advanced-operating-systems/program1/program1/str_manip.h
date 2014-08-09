// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file str_manip.h
/// @edit 2 Oct 2009
/// @brief Contains the function declarations for the str_manip library. This 
///   is a library of string manipulation functions to make working with 
///   strings much easier. Intended to be used along with the STL for strings.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_left( const string& s, const unsigned int& length ) const
/// @brief Returns a string of size length containing the left part of a string.
/// @pre A string exists.
/// @param const string& s - The string to get the left part of.
/// @param const unsigned int& length - The length of the string that will be
///   returned.
/// @post Returns a string of size length containing the left part of a string.
/// @return string of size length containing the left part of a string.
/// @note If length is greater than the length of s, then the entire string
///   will be returned. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_right( const string& s, const unsigned int& length ) const
/// @brief Returns a string of size length containing the right part of a string.
/// @pre A string exists.
/// @param const string& s - The string to get the right part of.
/// @param const unsigned int& length - The length of the string that will be
///   returned.
/// @post Returns a string of size length containing the right part of a string.
/// @return string of size length containing the right part of a string.
/// @note If length is greater than the length of s, then the entire string
///   will be returned. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_mid( const string& s, const unsigned int& start_pos, 
///               const unsigned int& length ) const
/// @brief Returns a string of size length containing the middle part of
///   a string, starting at position start_pos.
/// @pre A string exists.
/// @param const string& s - The string to get the middle part of.
/// @param const unsigned int& start_pos - The position in the string to start
///   getting from. Used as the number of characters over from the left side
///   of the string. 
/// @param const unsigned int& length - The length of the string that will be
///   returned.
/// @post Returns a string of size length containing the middle part of a string.
/// @return string of size length containing the middle part of a string.
/// @note If length is greater than the length of s, then the entire part of
///   the string to the right of start_pos will be returned.
/// @note if start_pos > length of s, will return s.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_remove( const string& s, const string& to_remove )
/// @brief Returns a string with all instances of to_remove deleted from s.
/// @pre A string exists. 
/// @param const string& s - The string to remove parts of.
/// @param const string& to_remove - The string to remove from s.
/// @post Returns a string with all instances of to_remove deleted from s.
/// @return string with all instances of to_remove deleted.
/// @note If to_remove is not found in s, s will be returned.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_remove( const string& s, const char& to_remove )
/// @brief Returns a string with all instances of to_remove deleted from s.
/// @pre A string exists. 
/// @param const string& s - The string to remove parts of.
/// @param const char& to_remove - The character to remove from s.
/// @post Returns a string with all instances of to_remove deleted from s.
/// @return string with all instances of to_remove deleted.
/// @note If to_remove is not found in s, s will be returned.
/// @note Just an overloaded version of the complete string version. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string str_clean( const string& s )
/// @brief Returns a clean version of string s. See notes below.
/// @pre A string exists.
/// @param const string& s - The string to clean.
/// @post Returns a clean version of s.
/// @return string containing the clean version of s.
/// @note Removes all non-alpha-numeric characters from s. 
/// @note If the string contains only special characters, a blank string ( "" )
///   will be returned. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn int str_to_int( const string& s )
/// @brief Returns a version of s converted to an integer.
/// @pre A string exists.
/// @param const string& s - The string to convert to an integer.
/// @post Returns a version of s converted to an integer.
/// @return int that is s converted to an integer. 
/// @note Wraps the STL atoi() function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn long int str_to_lint( const string& s )
/// @brief Returns a version of s converted to an long integer.
/// @pre A string exists.
/// @param const string& s - The string to convert to an long integer.
/// @post Returns a version of s converted to an long integer.
/// @return long int that is s converted to an long integer. 
/// @note Wraps the STL atol() function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn unsigned long int str_to_ulint( const string& s )
/// @brief Returns a version of s converted to an unsigned long integer.
/// @pre A string exists.
/// @param const string& s - The string to convert to an unsigned long integer.
/// @post Returns a version of s converted to an unsigned long integer.
/// @return unsigned long int that is s converted to an unsigned long integer. 
/// @note Wraps the STL strtoul() function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn double str_to_double( const string& s )
/// @brief Returns a version of s converted to a double.
/// @pre A string exists.
/// @param const string& s - The string to convert to a double.
/// @post Returns a version of s converted to a double.
/// @return double that is s converted to a double. 
/// @note Wraps the STL strtod() function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn char str_to_char( const string& s )
/// @brief Returns a version of s converted to a character.
/// @pre A string exists.
/// @param const string& s - The string to convert to a character array.
/// @post Returns a version of s converted to a character array.
/// @return char that is s converted to a character array. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string int_to_str( const int& n )
/// @brief Returns a version of n converted to a string.
/// @pre An integer exists.
/// @param const int& n - The interger to convert to a string.
/// @post Returns a version of n converted to a string. 
/// @return string that is n converted to a string. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string uint_to_str( const unsigned int& n )
/// @brief Returns a version of n converted to a string.
/// @pre An unsigned integer exists.
/// @param const ungsigned int& n - The ungsigned interger to convert to a string.
/// @post Returns a version of n converted to a string. 
/// @return string that is n converted to a string. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string double_to_str( const double& n )
/// @brief Returns a version of n converted to a string.
/// @pre A double exists.
/// @param const double& n - The double to convert to a string.
/// @post Returns a version of n converted to a string. 
/// @return string that is n converted to a string. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string char_to_str( const char& c )
/// @brief Returns a version of n converted to a string.
/// @pre A char exists.
/// @param const char& c - The char to convert to a string.
/// @post Returns a version of n converted to a string. 
/// @return string that is n converted to a string. 
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn vector<string> str_breakup( string s, const string& delim )
/// @brief Returns a vector of all segments in s delimited by delim.
/// @pre A vector<string> object exists.
/// @param string s - The string to look through and break apart.
/// @param const string& delim - The string to use as a marker to break apart
///   segments of s.
/// @post Returns a vector of all segments in s delimited by delim.
/// @return vector<string> object that is all the segments of s delimited by 
///   delim.
/// @note The returned object will have only one element in it containing s if
///   no instances of delim are found in s.
/// @note The delim string may contain multiple delimiters, I think.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn vector<unsigned int> str_find_all( const string& s, const string& find )
/// @brief Returns a vector of locations of all instances of find in s.
/// @pre A vector<string> object exists.
/// @param const string& s - The string to look through and break apart.
/// @param const string& delim - The string to use as a marker to break apart
///   segments of s.
/// @post Returns a vector of locations of all instances of find in s.
/// @return vector<unsigned int> object that is the location of each instance
///   of find in s.
/// @note Will not find overlapping instances of find. That is:
///   if s = radaradar and find = "radar" only "radar"adar will be found.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn vector<unsigned int> str_find_all( const strin& s, const string& find )
/// @brief Returns a vector of locations of all instances of find in s.
/// @pre A vector<string> object exists.
/// @param const string& s - The string to look through and break apart.
/// @param const char& delim - The char to use as a marker to break apart
///   segments of s.
/// @post Returns a vector of locations of all instances of find in s.
/// @return vector<unsigned int> object that is the location of each instance
///   of find in s.
/// @note Will not find overlapping instances of find. That is:
///   if s = radaradar and find = "radar" only "radar"adar will be found.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn long int bin_to_dec( const string& s )
/// @brief Returns a base 10 number from the binary string sent in.
/// @pre string s represents a binary number.
/// @param const string& s - The string to convert.
/// @post Returns a base 10 number from the binary string sent in.
/// @return long int that is the decimal equivilant of s' binary.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn long int oct_to_dec( const string& s )
/// @brief Returns a base 10 number from the octal string sent in.
/// @pre string s represents an octal number.
/// @param const string& s - The string to convert.
/// @post Returns a base 10 number from the octal string sent in.
/// @return long int that is the decimal equivilant of s' octal.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn long int hex_to_dec( const string& s )
/// @brief Returns a base 10 number from the hex string sent in.
/// @pre string s represents a hex number.
/// @param const string& s - The string to convert.
/// @post Returns a base 10 number from the hex string sent in.
/// @return long int that is the decimal equivilant of s' hex.
///////////////////////////////////////////////////////////////////////////////

#ifndef STR_MANIP_H
#define STR_MANIP_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>

#include "exception.h"
#include "useful_things.h"

using namespace std;

//Functions for getting part of a string
string str_left( const string& s, const unsigned int& length );
string str_right( const string& s, const unsigned int& length );
string str_mid( const string& s, const unsigned int& start_pos, 
          const unsigned int& length );
string str_mid_range( const string& s, const unsigned int& start_pos,
          const unsigned int& end_pos );

//Functions for removing part(s) of a string
string str_remove( const string& s, const string& to_remove );
string str_remove( const string& s, const char& to_remove );
string str_clean( const string& s );

//Functions for converting a string to other formats
int str_to_int( const string& s );
unsigned int str_to_uint( const string& s );
long int str_to_lint( const string& s );
unsigned long int str_to_ulint( const string& s );
double str_to_double( const string& s );
float str_to_float( const string& s );
const char* str_to_char( const string& s );

//Functions for converting other formats to strings
string int_to_str( const int& n );
string uint_to_str( const unsigned int& n );
string double_to_str( const double& n );
string float_to_str( const float& n );
string char_to_str( const char& c );

//Functions for finding parts of a string
vector<string> str_breakup( string s, const string& delim );
vector<unsigned int> str_find_all( const string& s, const string& find );
vector<unsigned int> str_find_all( const string& s, const char& find );

//Miscellaneous string functions
string str_toupper( const string& s );
string str_tolower( const string& s );
long int bin_to_dec( const string& s );
long int oct_to_dec( const string& s );
long int hex_to_dec( const string& s );

#endif

// END OF FILE ----------------------------------------------------------------
