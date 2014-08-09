#include "distributed_file.h"
#include "str_manip.h"

distributed_file::distributed_file() {
  last_entry.vn = 0;
  last_entry.ru = 0;
  strncpy( last_entry.text, "", sizeof("") );
}

distributed_file::~distributed_file() {
  // nada yo
}

void distributed_file::open() {
  file.open( path, fstream::in | fstream::app );
  if ( !file.is_open() ) {
    cout << "Could not open local file " << path << endl;
  }
}

void distributed_file::close() {
  file.close();
}

unsigned int distributed_file::flush() {
  unsigned int lines_written = 0;

  do {
    // check to see if entry with last entry vn + 1 exists (aka next plausible entry)
    map<unsigned int, distributed_file_entry_t>::iterator next_entry;
    next_entry = buffer.find( last_entry.vn + 1 );
    
    if ( next_entry == buffer.end() ) {
      return lines_written;
    }
    
    // if such an entry exists, append it to the file and update last entry
    last_entry = next_entry->second;
    
    open();
    // append vn
    file << last_entry.vn << delimiters;
    
    // append ru
    file << last_entry.ru << delimiters;
    
    // append ds
    for ( set<unsigned int>::iterator it = last_entry.ds.begin();
          it != last_entry.ds.end();
          it++ ) {
      file << " " << *it;     
    }
    file << delimiters;
    
    // append text
    file << last_entry.text << endl;
    
    close();  

    lines_written++;   
    
    buffer.erase( next_entry );
    
  } while ( true );
}

int distributed_file::append( const distributed_file_entry_t& entry ) {
  push( entry );
  flush();
}

distributed_file_entry_t distributed_file::get_last_entry() {
  return last_entry;
}

vector<distributed_file_entry_t> distributed_file::get_entries( unsigned int vn_first, unsigned int vn_last ) {
  vector<distributed_file_entry_t> entries;
  string line;
  // m_mutex.lock();
  vector<string> broken_line;
  vector<string> broken_ds;
  unsigned int line_vn = 0;
  
  open();
  
  while ( file.good() ) {
    std::getline( file, line );
    
    if ( line == "\n" || line == "" ) {
      continue;
    }
    
    // read version number of line. 
    broken_line = str_breakup( line, delimiters );
    line_vn = str_to_uint( broken_line[0] );
    
    // skip to the next entry
    if ( line_vn < vn_first ) {
      continue;
    }
    
    // stop reading. assumption: log file entries are sequential.
    else if ( line_vn > vn_last ) {
      break;
    }
    
    // parse it and push into entries to return
    else {
      distributed_file_entry_t next_entry;
      next_entry.vn = str_to_uint( broken_line[0] );
      next_entry.ru = str_to_uint( broken_line[1] );
      
      broken_ds = str_breakup( broken_line[2], " " );
      for ( unsigned int i = 0; i < broken_ds.size(); i++ ) {
        next_entry.ds.insert( str_to_uint( broken_ds[i] ) );
      }
      
      strncpy( next_entry.text, broken_line[3].c_str(), sizeof(broken_line[3].c_str()) );
      
      entries.push_back( next_entry );
    }
  }
  
  close();
  
  return entries;
}

unsigned int distributed_file::get_vn() const {
  return last_entry.vn;
}

unsigned int distributed_file::get_ru() const {
  return last_entry.ru;
}

set<unsigned int> distributed_file::get_ds() const {
  return last_entry.ds;
}

void distributed_file::lock() {
  m_mutex.lock();
}

void distributed_file::unlock() {
  m_mutex.unlock();
}

void distributed_file::print() {
  cout << "distributed_file at " << path <<  endl;
  print_distributed_file_entry( last_entry );
}

void distributed_file::push( const distributed_file_entry_t& entry ) {
  buffer[entry.vn] = entry;
}

void print_distributed_file_entry( const distributed_file_entry_t& entry ) {
  cout << "<" << entry.vn << ", " << entry.ru << ", ";
  for ( set<unsigned int>::iterator it = entry.ds.begin();
        it != entry.ds.end();
        it++ ) {
    cout << *it << " ";
  }  
  cout << ", " << entry.text << ">" << endl; 
}