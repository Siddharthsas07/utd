#ifndef NAIVE_BAYES_CLASSIFIER_H
#define NAIVE_BAYES_CLASSIFIER_H

#include <map>
#include <vector>
#include <string>
#include "DataSet.h"
#include "Document.h"
#include "Classifier.h"

using namespace std;

class NaiveBayesClassifier : public Classifier {
public:
  void train( const DataSet& data );
  float test( const DataSet& data );
  unsigned int classify( const Document& doc );  

protected:  
  vector<float> m_prior;
  map<string, vector<float> > m_condProb;
};

#endif
