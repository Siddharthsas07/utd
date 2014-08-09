// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file str_manip.cpp
/// @edit 2 Oct 2009
/// @brief Contains the function definitions for the str_manip library. 
///////////////////////////////////////////////////////////////////////////////

#include "str_manip.h"

using namespace std;

//Getting parts of strings ----------------------------------------------------
string str_left( const string& s, const unsigned int& length ) {
  return str_mid( s, 0, length );
}

string str_right( const string& s, const unsigned int& length ) {
  if ( length >= s.length() )
    return s;
    
  return s.substr( fun::bound( (unsigned int)(s.length()-length), (unsigned int)0, length ), length );
}

string str_mid( const string& s, const unsigned int& start_pos, 
                const unsigned int& length ) {    
  return s.substr( start_pos, length );
}

string str_mid_range( const string& s, const unsigned int& start_pos,
                      const unsigned int& end_pos ) {
  unsigned int start = fun::bound( start_pos, (unsigned int)0, (unsigned int)s.length() );
  unsigned int end   = fun::bound( end_pos, (unsigned int)0, (unsigned int)s.length() );

  if ( end < start ) {
    fun::swap( start, end );
  }

  return s.substr( start, end - start );
}

//Removing parts of strings ---------------------------------------------------
string str_remove( const string& s, const string& to_remove )
{
  //Check to make sure the string we want to remove is actually in s, if not
  //Then return s, because we didn't find anything to remove
  if ( s.find( to_remove ) == string::npos )
  {
    return s;
  }
  
  string curr = s;
  
  //Set our left and right indicies 
  unsigned int left = 0;
  unsigned int right = curr.length();
  
  //Get the left part that doesn't match our removal characters
  while ( str_mid ( curr, left, to_remove.length() ) == to_remove )
    left += to_remove.length();
  
  //Get the next part that doesn't match our removal characters
  while ( str_mid ( curr, right - to_remove.length(), to_remove.length() ) == to_remove )
    right -= to_remove.length();
    
  bool done = false;
  
  while ( !done )
  {
    unsigned int currpos = curr.find( to_remove, left );
    
    if ( currpos == string::npos || currpos > right )
      done = true;
    else
    {
      curr.erase( currpos, to_remove.length() );
      
      //Update right position to reflect that we removed part of the string
      right -= to_remove.length();
    }
  };
  
  return curr;
}

//Removes all instances of to_remove from s
string str_remove( const string& s, const char& to_remove )
{
  return str_remove( s, char_to_str( to_remove ) );
}

//Removes all instances of special characters from s
string str_clean( const string& s )
{
  string clean = "";
  for ( unsigned int i = 0; i < s.size(); i++ )
  {
    if ( isalnum( s[i] ) )
      clean += s[i];
  }
  
  return clean;
}

//Converting strings to other types -------------------------------------------
//String to integer
int str_to_int( const string& s ) {
  int n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to long integer
long int str_to_lint( const string& s ) {
  long int n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to unsigned integer
unsigned int str_to_uint( const string& s ) {
  unsigned int n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to unsigned long integer
unsigned long int str_to_ulint( const string& s ) {
  unsigned long int n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to double
double str_to_double( const string& s ) {
  double n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to double
float str_to_float( const string& s ) {
  float n;
  if ( !(istringstream(s) >> n) ) {
    n = 0;
  }

  return n;
}

//String to character
const char* str_to_char( const string& s ) {
  return s.c_str();
}

//Converting other types to strings -------------------------------------------
string int_to_str( const int& n ) {
  return static_cast<ostringstream*>( &(ostringstream() << n) )->str();
}

string uint_to_str( const unsigned int& n ) {
  return static_cast<ostringstream*>( &(ostringstream() << n) )->str();
}

string double_to_str( const double& n ) {
  return static_cast<ostringstream*>( &(ostringstream() << n) )->str();
}

string float_to_str( const float& n ) {
  return static_cast<ostringstream*>( &(ostringstream() << n) )->str();
}

string char_to_str( const char& c ) {
  string s( 1, c );
  return s;
}

//Functions for finding part of a string --------------------------------------
vector<string> str_breakup( string s, const string& delim ) {
  vector<string> lines;
  unsigned int lhs = 0;
  unsigned int rhs = s.find_first_of( delim, lhs );

  while ( rhs != string::npos ) {
    lines.push_back( s.substr( lhs, rhs-lhs ) );
    lhs = rhs + 1;
    rhs = s.find_first_of( delim, lhs );
  }

  return lines;
}

vector<unsigned int> str_find_all( const string& s, const string& find )
{
  vector<unsigned int> positions;
  unsigned int lhs = 0;
  unsigned int rhs = s.find( find, lhs );

  while ( rhs != string::npos ) {
    positions.push_back( rhs );
    lhs = rhs + 1;
    rhs = s.find( find, lhs );
  }

  return positions;
}

vector<unsigned int> str_find_all( const string& s, const char& find ) {
  return str_find_all( s, char_to_str( find ) );
}

//Miscellaneous functions -----------------------------------------------------
//Returns an uppercase version of s
string str_toupper( const string& s )
{
  string upper = "";
  for ( unsigned int i = 0; i < s.size(); i++ )
  {
    if ( isalpha( s[i] ) )
      upper += toupper( s[i] );
  }
  
  return upper;
}

//Returns a lowercase version of s
string str_tolower( const string& s )
{
  string lower = "";
  for ( unsigned int i = 0; i < s.size(); i++ )
  {
    if ( isalpha( s[i] ) )
      lower += tolower( s[i] );
  }
  
  return lower;
}

//Returns a decimal version of a string in binary
long int bin_to_dec( const string& s )
{
  return strtol( s.c_str(), NULL, 2 );
}

//Returns a decimal version of a string in octal
long int oct_to_dec( const string& s )
{
  return strtol( s.c_str(), NULL, 8 );
}

//Returns a decimal version of a string in hex
long int hex_to_dec( const string& s )
{
  return strtol( s.c_str(), NULL, 16 );
}

// END OF FILE ----------------------------------------------------------------
