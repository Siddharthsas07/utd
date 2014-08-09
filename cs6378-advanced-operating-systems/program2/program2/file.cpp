// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file File.cpp
/// @edit 5 Oct 2009
/// @brief Contains the function definitions for the File library. 
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
using std::ifstream;
using std::ofstream;

#include "file.h"
#include "exception.h"
#include "str_manip.h"

File::File() {
  m_filename = "";
}

File::File( const string& filename, bool keep_newline ) {
  m_filename = filename;
  load_file( keep_newline );
}

File::~File() {
  close();
}

void File::open( const string& filename, bool keep_newline ) {
  m_filename = filename;
  load_file( keep_newline );
  
  return;
}

void File::reopen() {
  load_file();
  
  return;
}

void File::save() {
  //Make an ostream for outputting
  ofstream os( m_filename.c_str(), ios::out | ios::app );

  //If the output failed, return
  if ( os.fail() ) {
    throw Exception( 10, "Couldn't save to output file: " + m_filename );
  }

  //Get the raw data from our file to output
  string out = get_raw( 0, get_num_lines() ); 
  
  //Write the data to disk
  os << out;
  
  //Close the file
  os.close();
  
  return;
}

void File::save( const string& filename ) {
  m_filename = filename;
  save();
  
  return;
}

void File::close( bool save_changes ) {
  if ( save_changes == true ) {
    save(); 
  }
  
  m_filename = "";
  clear();
    
  return;
}
  
const string& File::get_line( const unsigned int& line ) const {
  if ( line < get_num_lines() )
    return m_file[ line ];
  
  else {
    throw Exception( 50, "Invalid read from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }
} 

string& File::get_line( const unsigned int& line ) {
  if ( line < get_num_lines() )
    return m_file[ line ];
  
  else {
    throw Exception( 50, "Invalid read from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }
} 

const string& File::operator[]( const unsigned int& line ) const {
  if ( line < get_num_lines() )
    return m_file[ line ];
  
  else {
    throw Exception( 50, "Invalid read from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }
}

string& File::operator[]( const unsigned int& line ) {
  if ( line < get_num_lines() )
    return m_file[ line ];
  
  else {
    throw Exception( 50, "Invalid read from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }
}

void File::append( const string& s, bool break_lines ) {
  if ( break_lines == true ) {
    vector<string> lines = str_breakup( s, "\n" );
    
    m_file.insert( m_file.end(), lines.begin(), lines.end() );
  }

  else {
    m_file.push_back( s );
  }
}

void File::insert_line( const unsigned int& line, const string& s, bool break_lines ) {
  // Convert the string to a deque of 1 string
  insert_lines( line, deque<string>( 1, s ), break_lines );
  
  return;
}

void File::insert_lines( const unsigned int& line, const vector<string>& v, bool break_lines ) {
  // Convert the vector to a deque of string
  insert_lines( line, deque<string>( v.begin(), v.end() ), break_lines );
  
  return;
}

void File::insert_lines( const unsigned int& line, deque<string> d, bool break_lines ) {
  unsigned int temp = fun::bound( line, (unsigned int) 0, get_num_lines() );

  if ( break_lines ) {
    deque<string> ready;
    vector<string> lines;
    
    // Break each line in d on \n if specified, before putting into file contents
    for ( unsigned int i = 0; i < d.size(); i++ ) {
      lines = str_breakup( d[i], "\n" );      
      ready.insert( ready.end(), lines.begin(), lines.end() );
    }
    
    d = ready;
  }
  
  m_file.insert( m_file.begin() + line, d.begin(), d.end() );
  
  return;
}

void File::remove_line( const unsigned int& line ) {
  if ( line < get_num_lines() )
    m_file.erase( m_file.begin() + line );
  
  else {
    throw Exception( 60, "Invalid remove from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }

  return;
}

void File::remove_lines( const unsigned int& line, unsigned int n ) {
  if ( line < get_num_lines() ) {
    if ( line + n >= get_num_lines() ) {
      n = m_file.size() - line;
    }

    m_file.erase( m_file.begin() + line, m_file.begin() + line + n );
  }
   
  else {
    throw Exception( 60, "Invalid remove from file. Line " +
      uint_to_str( line ) + " does not exist in file \"" + 
      m_filename + "\"." );
  }

  return;
}

void File::set_data( const string& data ) {
  //Just convert data to a vector of size 1 with a string in it
  vector<string> v;
  v.push_back( data );
  set_data( v );
  
  return;
}

void File::set_data( const vector<string>& data ) {
  m_file.assign( data.begin(), data.end() );

  return;
}

void File::set_data( const deque<string>&  data ) {
  m_file = data;
  
  return;
}

const deque<string>& File::get_data() const {
  return m_file;
}

const vector<string>& File::get_data_vector() const
{
  vector<string> v;
  v.assign( m_file.begin(), m_file.end() );
  
  return v;
}

const string& File::get_raw( unsigned int start, unsigned int end )
{
  //If the file doesn'e exist, return a blank string
  if ( get_num_lines() == 0 ) {
    return "";
  }

  if ( end < start ) {
    fun::swap( start, end );
  }

  start = fun::bound( start, (unsigned int)0, get_num_lines() );
  end   = fun::bound( start, (unsigned int)0, get_num_lines() );
    
  string raw = "";
  
  //Loop through all of the data from start to end
  for ( unsigned int i = start; i <= end; i++ )
  {
    //Add it to the raw output
    raw += m_file[i];
    
    //And as long as we're not at the last line of the file, we want to add a \n
    //to each line's end so it will break up properly
    if ( i != m_file.size() - 1 )
      raw += char_to_str( '\n' );
  }
  
  return raw;
}

const string& File::get_filename() const {
  return m_filename;
}
  
unsigned int File::get_num_lines() const {
  return m_file.size();
}

bool File::empty() const {
  return m_file.empty();
}


void File::clear() {
  m_file.clear();

  return;
}

// IF file does not exist, create it, then blah blah
void File::load_file( bool keep_newline )
{
  //Clear the current data
  m_file.clear();
  
  //Create an in stream for the contents of the file
  ifstream is( m_filename.c_str(), ios::in | ios::out | ios::app );
  
  //If the output failed, return
  if ( is.fail() ) { 
    throw Exception( 20, "Couldn't open the file: " + m_filename ); 
  } 
  
  string file;
  string line;
  
  //As long as we're not at the end of the file
  while ( !is.eof() ) {
    getline( is, line );

    if ( keep_newline ) {
      line += char_to_str( '\n' );
    }

    m_file.push_back( line );
  };
  
  return;
}

// END OF FILE ----------------------------------------------------------------
