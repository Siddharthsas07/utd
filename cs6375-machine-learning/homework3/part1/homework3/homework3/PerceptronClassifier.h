#ifndef PERCEPTRON_CLASSIFIER_H
#define PERCEPTRON_CLASSIFIER_H

#include <map>
#include <vector>
#include <string>
#include "DataSet.h"
#include "Document.h"
#include "Classifier.h"

using namespace std;

class PerceptronClassifier : public Classifier {
public:
  void train( const DataSet& data );
  float test( const DataSet& data );
  int classify( const Document& doc );  

protected:  
  void createVocabToVectorMap();
  void updateWeights();

  double dot( const vector<double>& v, const vector<int>& u );

  unsigned int m_numDocs;
  unsigned int m_numAttrs;

  Dictionary m_vocabToVector;

  vector<vector<int> > m_data;
  vector<double> m_w;
  vector<double> m_error;
};

#endif