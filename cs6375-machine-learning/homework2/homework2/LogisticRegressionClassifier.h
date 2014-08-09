#ifndef LOGISTIC_REGRESSION_CLASSIFIER_H
#define LOGISTIC_REGRESSION_CLASSIFIER_H

#include <map>
#include <vector>
#include <string>
#include "DataSet.h"
#include "Document.h"
#include "Classifier.h"

using namespace std;

class LogisticRegressionClassifier : public Classifier {
public:
  void train( const DataSet& data );
  float test( const DataSet& data );
  unsigned int classify( const Document& doc );  

protected:  
  void createVocabToVectorMap();
  void gradientAscent();
  double pHat( unsigned int idx );

  unsigned int m_numDocs;
  unsigned int m_numAttrs;

  Dictionary m_vocabToVector;

  vector<vector<unsigned int> > m_data;
  vector<double> m_w;
  vector<double> m_dw;
  vector<double> m_pr; 
};

#endif
