#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include "useful_things.h"
#include "DataSet.h"
#include "stringHelp.h"

bool DataSet::loadData( string dataPath, string fileName ) {
  try {
    #ifdef WINDOWS
      dataPath = str_replace_all( dataPath, "/", "\\", ':' );
      fileName = str_replace_all( fileName, "/", "\\", ':' );
    #endif

    cout << "Loading data set index file " << dataPath << fileName << endl;

    string line;
 
    // Load class index file and data locations.
    ifstream indexFile( (dataPath + fileName).c_str() );    
    if ( indexFile.is_open() ) {
      m_classes.clear();
     
      while ( indexFile.good() ) {
        pair<string,string> classInfo;
        getline( indexFile, line );
        classInfo.first = line;
        getline( indexFile, line );
        classInfo.second = line;
        #ifdef WINDOWS
        classInfo.first = str_replace_all( classInfo.first, "/", "\\", NULL );
        classInfo.second = str_replace_all( classInfo.second, "/", "\\", NULL );
        #endif
        m_classes.push_back( classInfo );
      }
      indexFile.close();
    }    

    // Create class file list. Store as classname.txt.
    for ( unsigned int i = 0; i < m_classes.size(); i++ ) {
      string cmd;

      #ifdef WINDOWS
      cmd = "dir " + dataPath + m_classes[i].second
        + " /b > " + m_classes[i].first + ".txt";
      #endif

      #ifdef GCC
      cmd = "ls " + dataPath + m_classes[i].second 
        + " > " + m_classes[i].first + ".txt";
      #endif

      cout << "Executing: " << cmd << endl;
      system( cmd.c_str() );
    }

    // Load data from files in class file list. Store as Document objs.
    for ( unsigned int i = 0; i < m_classes.size(); i++ ) {
      ifstream classDocList( (m_classes[i].first + ".txt").c_str() );

      while ( classDocList.good() ) {
        m_docs.resize( m_docs.size() + 1 );

        getline( classDocList, line ); // line contains file name
        if ( line != "" ) {
          string currDocPath = dataPath + m_classes[i].second + line;
          ifstream document( currDocPath.c_str() );
          if ( document.is_open() ) {
            stringstream buffer;
            buffer << document.rdbuf();
            m_docs[i].resize( m_docs[i].size() + 1 );
            m_docs[i][m_docs[i].size()-1].m_contents = buffer.str();
            m_docs[i][m_docs[i].size()-1].m_loc = currDocPath;
          }
        }
      }
      classDocList.close();
    }    
  }

  catch ( exception& e ) {
    cout << e.what() << endl;
    return false;
  }

  return true;
}