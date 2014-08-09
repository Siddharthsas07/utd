#ifndef DISTRIBUTED_FILE_H
#define DISTRIBUTED_FILE_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <string>

#include "Mutex.h"

using std::vector;
using std::string;
using std::fstream;
using std::map;
using std::set;
using std::string;

typedef struct {
  unsigned int      vn;
  unsigned int      ru;
  set<unsigned int> ds;
  char              text[64];
} distributed_file_entry_t;

typedef struct {
  unsigned int      m;
  set<unsigned int> i;
  unsigned int      n;
} distinguished_t;

typedef map<unsigned int, distributed_file_entry_t> vn_list_t;
typedef map<unsigned int, distributed_file_entry_t> entry_list_t;

class distributed_file {
  protected:
    Mutex m_mutex;
    
    void push( const distributed_file_entry_t& entry );
    unsigned int flush();

  public:
    distributed_file_entry_t last_entry;
    entry_list_t             buffer;
    string                   path;
    fstream                  file;
    string                   delimiters;
  
    distributed_file();
    ~distributed_file();
  
    void open();
    void close();
  
    int                              append( const distributed_file_entry_t& entry );
    distributed_file_entry_t         get_last_entry();
    vector<distributed_file_entry_t> get_entries( unsigned int vn_first, unsigned int vn_last );
    
    unsigned int                     get_vn() const;
    unsigned int                     get_ru() const;
    set<unsigned int>                get_ds() const;  
    
    void lock();
    void unlock();
    void print();
};

void print_distributed_file_entry( const distributed_file_entry_t& entry );

#endif