#ifndef CONVERTER_H
#define CONVERTER_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>

#include "stringHelp.h"
#include "useful_things.h"

using namespace std;

typedef std::map<string, unsigned int> Dictionary;

class Converter {
public:
  string       m_dataPath;     // Path to data directory
  string       m_trainIndex;   // Name of train index file
  string       m_testIndex;    // Name of test index file
  string       m_ignoreIndex;  // Name of ignore words index file
  string       m_delimiters;   // Delimiters to tokenize input
  unsigned int m_ignoreLength; // Smallest acceptable toekn length for input

  // Full-value constructor without ignore words
  Converter( const string& dataPath, 
    const string& trainIndex, 
    const string& testIndex,
    const string& delimiters = "",
    const unsigned int ignoreLength = 0 );

  // Full-value constructor with ignore words
  Converter( const string& dataPath, 
    const string& trainIndex, 
    const string& testIndex,
    const string& ignoreIndex,
    const string& delimiters = "",
    const unsigned int ignoreLength = 0 );

  // Read input files, convert to ARFF, output to outPath
  void outputARFF( const string& outPath );

protected:
  string                        m_tempFileName;
  multimap<string, string>      m_classIndex;
  vector<pair<string, string> > m_fileIndex;
  vector<Dictionary>            m_fileVocab;
  set<string>                   m_globalVocab;
  vector<string>                m_ignoreVocab;

  void f_outputARFF( const string& inLoc, const string& outLoc );
  void createFileVocab( ifstream& file, Dictionary& vocab );
  void createTestFileVocab( ifstream& file, Dictionary& vocab );

};

#endif
