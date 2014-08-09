#ifndef CRITICAL_SECTION_TRACKER_H
#define CRITICAL_SECTION_TRACKER_H

#include <vector>
#include "Mutex.h"

using std::vector;

// Simply a class for gouping variables related to critical section entry
class CriticalSectionTracker {
  public:
    Mutex             shared_vars;
    
    unsigned int      seq_num;
    unsigned int      max_seq_num;
    
    bool              requesting_cs;
    bool              defer_it;
    
    unsigned int      outstanding_rep_count;
    vector<bool>      rep_deferred;
    
    vector<bool>      mutex_tokens;
    
    ConditionVariable entry_ok;
    Mutex             entry_ok_mutex;
    
    CriticalSectionTracker() : 
      seq_num(0),
      max_seq_num(0),
      requesting_cs(false),
      defer_it(false),
      outstanding_rep_count(0) 
    {}
    ~CriticalSectionTracker() {}
};

#endif