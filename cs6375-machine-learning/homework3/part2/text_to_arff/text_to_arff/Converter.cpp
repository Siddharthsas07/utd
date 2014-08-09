#include "Converter.h"

using namespace std;

Converter::Converter( const string& dataPath, 
                      const string& trainIndex, 
                      const string& testIndex,
                      const string& delimiters,
                      const unsigned int ignoreLength ) {
  #ifdef WINDOWS
    m_dataPath = str_replace_all( dataPath, "/", "\\", ':' );
    m_trainIndex = str_replace_all( trainIndex, "/", "\\", ':' );
    m_testIndex = str_replace_all( testIndex, "/", "\\", ':' );
  #else
    m_dataPath = dataPath;
    m_trainIndex = trainIndex;
    m_testIndex = testIndex;
  #endif
  m_ignoreIndex = "";
  m_delimiters = delimiters;
  m_ignoreLength = ignoreLength;
  m_tempFileName = "tempARFFConverter";
}

Converter::Converter( const string& dataPath, 
                      const string& trainIndex, 
                      const string& testIndex,
                      const string& ignoreIndex,
                      const string& delimiters,
                      const unsigned int ignoreLength ) {
  #ifdef WINDOWS
    m_dataPath = str_replace_all( dataPath, "/", "\\", ':' );
    m_trainIndex = str_replace_all( trainIndex, "/", "\\", ':' );
    m_testIndex = str_replace_all( testIndex, "/", "\\", ':' );
    m_ignoreIndex = str_replace_all( ignoreIndex, "/", "\\", ':' );
  #else
    m_dataPath = dataPath;
    m_trainIndex = trainIndex;
    m_testIndex = testIndex;
  #endif
  m_delimiters = delimiters;
  m_ignoreLength = ignoreLength;
  m_tempFileName = "tempARFFConverter";
}

void Converter::outputARFF( const string& outPath ) {
  // Read ignore tokens from file if necessary
  if ( m_ignoreIndex != "" ) {
    string line;
    ifstream ignoreFile( (m_dataPath+m_ignoreIndex).c_str() );  
    if ( ignoreFile.is_open() ) {
      while ( ignoreFile.good() ) {
        getline( ignoreFile, line );
        if ( line != "" ) {
          m_ignoreVocab.push_back( line );
        }
      }
      ignoreFile.close();
    }
    else {
      cout << "Unable to open input file: " << m_dataPath+m_ignoreIndex << endl;
    }
  }

  f_outputARFF( m_dataPath + m_trainIndex, outPath + m_trainIndex + "_ARFF" );
  f_outputARFF( m_dataPath + m_testIndex, outPath + m_testIndex + "_ARFF" );
}

void Converter::f_outputARFF( const string& inLoc, const string& outLoc ) {
  string line;

  ifstream in_classIndex( inLoc.c_str() );  
  // Index directories of files and the class for each directory
  if ( in_classIndex.is_open() ) {
    m_classIndex.clear();
    pair<string,string> classInfo;
    while ( in_classIndex.good() ) {      
      getline( in_classIndex, line );
        classInfo.first = line;
      getline( in_classIndex, line );
        classInfo.second = line;
      #ifdef WINDOWS
        classInfo.second = str_replace_all( classInfo.second, "/", "\\", NULL );
      #endif
      m_classIndex.insert( classInfo );
    }
  }
  else {
    cout << "Unable to open input file: " << m_dataPath+m_trainIndex << endl;
    return;
  }
  in_classIndex.close();

  // Generate list of input files for each class path
  m_fileIndex.clear();
  for ( multimap<string, string>::iterator it = m_classIndex.begin(); it != m_classIndex.end(); it++ ) {    
    string cmd;
    #if defined(WINDOWS)
      cmd = "dir " + m_dataPath + it->second + " /b > " + m_tempFileName;
    #elif defined(LINUX) || defined(UNIX)
      cmd = "ls " + m_dataPath + it->second + " > " + m_tempFileName;
    #else
      printf("Converter doesn't know how to work on this operating system.\n");
      return;
    #endif
    cout << "Executing: " << cmd << endl;
    system( cmd.c_str() );

    // Now that the files are listed in temp file, read them into the fileIndex
    ifstream in_fileIndex( m_tempFileName.c_str() );
    if ( in_fileIndex.is_open() ) {      
      pair<string,string> fileInfo;
      while ( in_fileIndex.good() ) {        
        getline( in_fileIndex, line );
        if ( line != "" && line != "\n" ) {
          fileInfo.first = it->first;
          fileInfo.second = m_dataPath + (*it).second + line;
          #ifdef WINDOWS
            fileInfo.second = str_replace_all( fileInfo.second, "/", "\\", NULL );
          #endif
          m_fileIndex.push_back( fileInfo );
        }
      }
    }
    else {
      cout << "Unable to open input file: " << m_tempFileName << endl;
    }
    in_fileIndex.close();
  }

  // delete temp file
  if( remove( m_tempFileName.c_str() ) != 0 ) {
    cout << "Unable to delete file: " << m_tempFileName << endl;
  }

  // Open each file in the file index and
  // Add tokens to global vocab and to file's dictionary
  m_fileVocab.resize( m_fileIndex.size() );
  
  if ( m_globalVocab.empty() ) {
    for ( unsigned int i = 0; i < m_fileIndex.size(); i++ ) {
      ifstream currFile( (m_fileIndex[i].second).c_str() );
      if ( currFile.is_open() ) {
        createFileVocab( currFile, m_fileVocab[i] );
      }
      else {
        cout << "Unable to open input file: " << m_fileIndex[i].second << endl;
      }
      currFile.close();
    }
  }
  else {
    for ( unsigned int i = 0; i < m_fileIndex.size(); i++ ) {
      ifstream currFile( (m_fileIndex[i].second).c_str() );
      if ( currFile.is_open() ) {
        createTestFileVocab( currFile, m_fileVocab[i] );
      }
      else {
        cout << "Unable to open input file: " << m_fileIndex[i].second << endl;
      }
      currFile.close();
    }
  }

  // Now that we have the global vocab and local vocabs...
  // Create header section of output file.
  ofstream arffFile;
  arffFile.open( outLoc.c_str() );
  if ( arffFile.is_open() ) {
    // RELATION line
    arffFile << "@RELATION ham_spam\n\n";

    // ATTRIBUTE lines
    for ( set<string>::iterator it = m_globalVocab.begin(); it != m_globalVocab.end(); it++ ) {
      arffFile << "@ATTRIBUTE t_" << *it << " NUMERIC\n";
    }
    arffFile << "@ATTRIBUTE class {";
    set<string> uniqueClasses;
    for ( multimap<string, string>::iterator it = m_classIndex.begin(); it != m_classIndex.end(); it++ ) {
      uniqueClasses.insert( it->first );
    }    
    for ( set<string>::iterator it = uniqueClasses.begin(); it != uniqueClasses.end(); it++ ) {
      arffFile << *it;
      if ( distance(it, uniqueClasses.end()) > 1 ) {
        arffFile << ",";
      }
    }
    arffFile << "}\n\n";

    // DATA lines
    arffFile << "@DATA" << endl;
    for ( unsigned int i = 0; i < m_fileVocab.size(); i++ ) {
      arffFile << "{";
      for ( Dictionary::iterator it = m_fileVocab[i].begin(); it != m_fileVocab[i].end(); it++ ) {
        unsigned int dist = distance( m_globalVocab.begin(), m_globalVocab.find( it->first ) );
        arffFile << dist << " " << it->second << ", ";
      }
      arffFile << m_globalVocab.size() << " " << m_fileIndex[i].first << "}\n";
    }

    arffFile.close();
  }
  else {
    cout << "Unable to open output file: " << outLoc << endl;
  }
}

void Converter::createFileVocab( ifstream& file, Dictionary& vocab ) {
  // Tokenize document and add vocab!
  stringstream fileBuffer;
  fileBuffer << file.rdbuf();
  string fileAsString = fileBuffer.str();
  unsigned int lhs = 0;
  unsigned int rhs = fileAsString.find_first_of( m_delimiters, lhs );
  vector<string>::iterator ignoreIt;
  string token;
  vocab.clear();

  // This is the first time this function was called. Training data was created first.
  while ( rhs != string::npos ) {
    token = fileAsString.substr( lhs, rhs-lhs );
    if ( (token.length() >= m_ignoreLength) 
      && (find( m_ignoreVocab.begin(), m_ignoreVocab.end(), token ) == m_ignoreVocab.end()) ) 
    {
      vocab[token]++;
      m_globalVocab.insert(token);
    }
    lhs = rhs+1;
    rhs = fileAsString.find_first_of( m_delimiters, lhs );
  }
}

void Converter::createTestFileVocab( ifstream& file, Dictionary& vocab ) {
  // Tokenize document and add vocab!
  stringstream fileBuffer;
  fileBuffer << file.rdbuf();
  string fileAsString = fileBuffer.str();
  unsigned int lhs = 0;
  unsigned int rhs = fileAsString.find_first_of( m_delimiters, lhs );
  vector<string>::iterator ignoreIt;
  string token;
  vocab.clear();

  // This is the first time this function was called. Training data was created first.
  while ( rhs != string::npos ) {
    token = fileAsString.substr( lhs, rhs-lhs );
    if ( (token.length() >= m_ignoreLength) 
      && (find( m_globalVocab.begin(), m_globalVocab.end(), token ) != m_globalVocab.end()) ) 
    {
      vocab[token]++;
    }
    lhs = rhs+1;
    rhs = fileAsString.find_first_of( m_delimiters, lhs );
  }
}