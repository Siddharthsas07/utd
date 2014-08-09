// BEGINNING OF FILE ----------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
/// @auth Gary Steelman
/// @file File.h
/// @edit 5 Oct 2009
/// @brief Contains the function declarations for the File library. These
///   functions are intended to make working with files through C++ easier. 
/// @note Relies on the "str_manip.h" library and the "exception.h" library.
/// @note Might expand later to have assignment operators, copy constructor
///   until then do NOT try to assign one File to another!
/// @note Still in progress! Going to add stream operators eventually.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @class File
/// @brief Creates an object with the functionality of the File class.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn File()
/// @brief Default constructor for the File class.
/// @pre None.
/// @post Creates an object of type File.
/// @return N/A.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn File( const string& filename, bool keep_newline = true )
/// @brief Constructor for the File class that creates an object with the data
///   found in file.
/// @pre file exists.
/// @param const string& filename - The location of the file to open.
/// @param bool keep_newline = true - Pass true (or don't pass) to keep the 
///   new line ( \n ) character at the end of strings when read from a file.
///   Pass false to not keep the new line character at the end of each string.
/// @post Creates an object of type File with the data in file.
/// @return N/A.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn ~File()
/// @brief Default destructor for the File class.
/// @pre A File object exists.
/// @post Frees the memory used by the object.
/// @return N/A.
/// @note Does NOT save changes, be sure to use the save() function before 
///   destruction to save your changes!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void open( const string& filename )
/// @brief Opens the file located at filename and sets File data to the file's
///   contents.
/// @pre filename exists.
/// @param const string& filename - The location of the file to open.
/// @post Opens the file located at filename and sets File data to the file's 
///   contents.
/// @return None.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void reopen()
/// @brief Reopens the file.
/// @pre filename exists.
/// @param const string& filename - The location of the file to open.
/// @post Reopens the file. 
/// @return None.
/// @note Just reloads the original file data from disk.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void save()
/// @brief Saves the current file data to the current file path.
/// @pre A File object exists. 
/// @post Saves the current file data to the current file path.
/// @return None.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void save( const string& filename )
/// @brief Saves the current file data to the path specified by filename.
/// @pre filename exists.
/// @param const string& filename - The location of the file to save to.
/// @post Saves the current file data to the path specified by filename.
/// @return None.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void close( bool save = false )
/// @brief Closes the current file, saving the data if specified.
/// @pre A File object exists.
/// @param bool save - Defaults to false meaning changes are NOT saved. Pass
///   true to save changes.
/// @post Closes the current file, saving the data if specified.
/// @return None.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string get_line( const unsigned int& line ) const
/// @brief Returns the specified line from the file data. 
/// @pre A File object exists.
/// @param const unsigned int& line - The line number to get from the file.
/// @post Returns the specified line from the file data. 
/// @return string& object that is the line specified from the file.
/// @throw Exception 50.
/// @note 0 is first line.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string& operator[]( const unsigned int& line )
/// @brief Returns the specified line from the file data. 
/// @pre A File object exists.
/// @param const unsigned int& line - The line number to get from the file.
/// @post Returns the specified line from the file data. 
/// @return string& object that is the line specified from the file.
/// @throw Exception 50.
/// @note 0 is first line.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void insert_line( const unsigned int& line, const string& s, bool break_lines = true )
/// @brief Inserts s into line and breaks s into multiple lines while doing it
///   at any \n characters if break_lines is true. 
/// @pre A File object exists.
/// @param const unsigned int& line - The line number to insert to in the file.
/// @param const string& s - The string to write to the file.
/// @param bool break_lines - True to break s at \n when inserting, false to not.
/// @post Inserts s into line in File and breaks s into multiple lines if 
///   specified. 
/// @return None.
/// @note 0 is first line.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void insert_lines( const unsigned int& line, const vector<string>& v, bool break_lines = true )
/// @brief Inserts v into File at line and breaks each element in v into
///    multiple lines while doing it at any \n characters if break_lines is true. 
/// @pre A File object exists.
/// @param const unsigned int& line - The line number to insert to in the file.
/// @param const vector<string>& v - The strings to write to File data. 
/// @param bool break_lines - True to break s at \n when inserting, false to not.
/// @post Inserts v into File at line and breaks each element in v into multiple 
///   lines while doing it at any \n characters if break_lines is true.
/// @return None.
/// @note 0 is first line.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void insert_lines( const unsigned int& line, const deque<string>& d, bool break_lines = true )
/// @brief Inserts d into File at line and breaks each element in v into
///    multiple lines while doing it at any \n characters if break_lines is true. 
/// @pre A File object exists.
/// @param const unsigned int& line - The line number to insert to in the file.
/// @param const deque<string>& d - The strings to write to File data. 
/// @param bool break_lines - True to break s at \n when inserting, false to not.
/// @post Inserts v into File at line and breaks each element in v into multiple 
///   lines while doing it at any \n characters if break_lines is true.
/// @return None.
/// @note 0 is first line.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void remove_line( const unsigned int& line )
/// @brief Removes line number specified from File data. 
/// @pre A file object exists and has opened a file.
/// @param const unsigned int& line - The line number to remove.
/// @post Removes line number specified from File data.
/// @return None.
/// @throw Exception 60.
/// @note 0 is first line.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void remove_lines( const unsigned int& line, const unsigned int& n )
/// @brief Removes lines from File data starting at line and going for n lines. 
/// @pre A file object exists and has opened a file.
/// @param const unsigned int& line - The line to start removing at.
/// @param const unsigned int& n - The number of lines to remove. 
/// @post Removes line number specified from File data.
/// @return None.
/// @note If n would make the removal go past the last line in the file, instead
///   it just removes through the end of the file. 
/// @throw Exception 60.
/// @note 0 is first line.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void set_data( const string& data )
/// @brief Sets the File data to data.
/// @pre A File object exists.
/// @param const string& data - The string to set File data to.
/// @post Sets the File data to data.
/// @return None.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void set_data( const vector<string>& data )
/// @brief Sets the File data to data.
/// @pre A File object exists.
/// @param const vector<string>& data - The strings to set File data to.
/// @post Sets the File data to data.
/// @return None.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void set_data( const deque<string>&  data )
/// @brief Sets the File data to data.
/// @pre A File object exists.
/// @param const deque<string>& data - The strings to set File data to.
/// @post Sets the File data to data.
/// @return None.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn deque<string> get_data() const
/// @brief Returns the File data in a deque<string> object. One line per index.
/// @pre A File object with File data exists.
/// @post Returns the File data in a deque<string> object. One line per index.
/// @return deque<string> object that has the File data. One line per index.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn vector<string> get_data() const
/// @brief Returns the File data in a vector<string> object. One line per index.
/// @pre A File object with File data exists.
/// @post Returns the File data in a vector<string> object. One line per index.
/// @return vector<string> object that has the File data. One line per index.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string get_raw( unsigned int start = 0, unsigned int end - =1 )
/// @brief Returns the raw data contained in File data.
/// @pre A File object with File data exists. 
/// @param unsigned int start - The line to start getting raw data from in File.
/// @param unsigned int end - The line to stop getting raw data from in File.
/// @post Returns the raw data contained in File data.
/// @return string containing all of the data in File from line start to end.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn string get_filename() const
/// @brief Returns the filename currently associate with File.
/// @pre A File object exists.
/// @post Returns the filename currently associate with File.
/// @return string that is the location of the filename currently associated
///   with File.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn unsigned int get_num_lines() const
/// @brief Returns the number of lines in File data.
/// @pre A File object exists.
/// @post Returns the number of lines in File data.
/// @return unsigned int that is the number of lines in File data.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn bool get_error() const
/// @brief Returns if there was an error in the last operation performed by a 
///   File open, close, insert, or remove function.
/// @pre A File object exists.
/// @post Returns if there was an error in the last File operation.
/// @return bool that is if the last operation errored or not. True for
///   an error, false if no error.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn bool empty() const
/// @brief Returns if the File data is empty or not.
/// @pre A File object exists.
/// @post Returns if the File data is empty or not.
/// @return bool that is if the File data is empty or not. True for empty,
///   false otherwise.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void clear()
/// @brief Clears the File data.
/// @pre A File object exists.
/// @post Clears the File data.
/// @return None.
/// @note Does not write directly to disk! You must save() to write to disk!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @fn void load_file( bool keep_newline = true )
/// @brief Opens the file located at filename and sets File data to the file's
///   contents.
/// @pre A File object exists.
/// @param bool keep_newline - True to keep the \n characters in the input 
///   stream. False to clean the \n characters when loading.
/// @post Opens the file located at filename and sets File data to the file's
///   contents.
/// @return None.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @Exceptions
/// 10 - Invalid save to file. Can't create outstream to file. 
/// 20 - Invalid file to open. Can't access file to read. 
/// 50 - Invalid read from file. Line x does not exist in file y.
/// 60 - Invalid remove from file. Line x does not exist in file y.
///////////////////////////////////////////////////////////////////////////////

#pragma once //Ensures this file is only included once during compilation

#include <string>
using std::string;

#include <deque>
using std::deque;

#include <vector>
using std::vector;

#include "useful_things.h"

class File
{
  public:
    File();
    File( const string& filename, bool keep_newline = false );
    ~File();
    
    void open( const string& filename, bool keep_newline = false );
    void reopen();
    
    void save();
    void save( const string& filename );
    
    void close( bool save = false );
    
    const string& get_line( const unsigned int& line ) const;
    string& get_line( const unsigned int& line );
    const string& operator[]( const unsigned int& line ) const;
    string& operator[]( const unsigned int& line );    
    
    void append( const string& s, bool break_lines = true );

    void insert_line( const unsigned int& line, const string& s, bool break_lines = true );
    void insert_lines( const unsigned int& line, const vector<string>& v, bool break_lines = true );
    void insert_lines( const unsigned int& line, deque<string> d, bool break_lines = true );
    
    void remove_line( const unsigned int& line );
    void remove_lines( const unsigned int& line, unsigned int n );
    
    void set_data( const string& data );
    void set_data( const vector<string>& data );
    void set_data( const deque<string>&  data );
    
    const deque<string>& get_data() const;
    const vector<string>& get_data_vector() const;
    const string& get_raw( unsigned int start = 0, unsigned int end = -1 );
    
    const string& get_filename() const;
    unsigned int get_num_lines() const;
    
    bool empty() const;
    void clear();
    
  protected:
    void load_file( bool keep_newline = true );
    
  private:
    deque<string> m_file;
    string m_filename;  
};

// END OF FILE ----------------------------------------------------------------
