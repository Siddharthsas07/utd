#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "useful_things.h"
#include "Classifier.h"
#include "stringHelp.h"

const string ignoreFileLoc = "../data/ignore.txt";
const string stopFileLoc = "../data/stopwords.txt";
const bool useIgnoreList = true;
const bool useStopWordsList = true;
const string vocabDelimiters = "\n -:;!@#$%^&*()=+[]{}<>,.?/\\\"'|";
const unsigned int minTokenSize = 3;

void Classifier::tokenizeDocument( const Document& doc, 
                                   string delimiters, 
                                   vector<string>& ignore,
                                   unsigned int minSize,
                                   vector<string>& split ) {
  split.clear();
  unsigned int lhs = 0;
  unsigned int rhs = doc.m_contents.find_first_of( delimiters, lhs );
  vector<string>::iterator it;

  // Iterate over string, grabbing each token and keeping ones that meet
  // criteria. 
  while ( rhs != string::npos ) { 
    string token = doc.m_contents.substr( lhs, rhs-lhs );
    if ( token.length() >= minSize ) {
      it = find( ignore.begin(), ignore.end(), token ); 
      if ( it == ignore.end() ) {
        split.push_back( token );
      }      
    }
    lhs = rhs+1;
    rhs = doc.m_contents.find_first_of( delimiters, lhs );
  }
}

bool Classifier::addVocab( const vector<Document>& docs, 
                           map<string, unsigned int>& vocab ) {
  // Construct list of tokens to ignore.
  vector<string> tokensToIgnore;
  try {
    if ( useIgnoreList == true ) {
      string tokenFileLoc = ignoreFileLoc;
      #ifdef WINDOWS
        tokenFileLoc = str_replace_all( tokenFileLoc, "/", "\\", ':' );
      #endif
      string line;
      ifstream tokenFile( tokenFileLoc.c_str() );
      
      while ( tokenFile.good() ) {
        getline( tokenFile, line );
        if ( line != "" ) {
          tokensToIgnore.push_back( line );
        }
      }
      tokenFile.close();
    }
    if ( useStopWordsList == true ) {
      string tokenFileLoc = stopFileLoc;
      #ifdef WINDOWS
        tokenFileLoc = str_replace_all( tokenFileLoc, "/", "\\", ':' );
      #endif
      string line;
      ifstream tokenFile( tokenFileLoc.c_str() );
      
      while ( tokenFile.good() ) {
        getline( tokenFile, line );
        if ( line != "" ) {
          tokensToIgnore.push_back( line );
        }
      }
      tokenFile.close();
    }
  }

  catch ( exception& e ) {
    cout << e.what() << endl;
    return false;
  }
  
  for ( unsigned int i = 0; i < docs.size(); i++ ) {
    vector<string> tokens;
    tokenizeDocument( docs[i], vocabDelimiters, tokensToIgnore, minTokenSize, tokens );

    for ( unsigned int j = 0; j < tokens.size(); j++ ) {
      vocab[tokens[j]]++;
    }
  }

  return true;
}

bool Classifier::addVocab( const Document &doc, 
                           map<string,unsigned int> &vocab ) {
  vector<Document> documentList;
  documentList.push_back( doc );
  return addVocab( documentList, vocab );
}

unsigned int Classifier::sumTokens( map<string, unsigned int>& vocab ) {
  map<string, unsigned int>::iterator it;
  unsigned int count = 0;
  // Sum the number of tokens in the vocabulary.
  for ( it = vocab.begin(); it != vocab.end(); it++ ) {
    count += (it->second);
  }
  return count;
}
