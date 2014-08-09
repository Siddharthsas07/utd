#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <map>
#include <vector>
#include <string>
#include "DataSet.h"
#include "Document.h"

typedef map<string, unsigned int> Dictionary;

using namespace std;

class Classifier {
public:
  virtual void train( const DataSet& data ) = 0;
  virtual float test( const DataSet& data ) = 0;
  virtual unsigned int classify( const Document& doc ) = 0;  

protected:
  bool addVocab( const vector<Document>& docs, Dictionary& vocab );
  bool addVocab( const Document& doc, Dictionary& vocab );

  void tokenizeDocument( const Document& str,
    string delimiters,
    vector<string>& ignore,
    unsigned int minSize,
    vector<string>& split );

  unsigned int sumTokens( Dictionary& vocab );

  vector<string> m_classes;
  Dictionary m_vocab;
  vector<Dictionary> m_classVocab;
};

#endif
