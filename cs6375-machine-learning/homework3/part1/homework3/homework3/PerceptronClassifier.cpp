#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "PerceptronClassifier.h"
#include "stringHelp.h"
#include "hr_timer.h"

const unsigned int weightUpdateIterationsLimit = 25;
const double w_0 = 1.0;
const double eta = .1;

void PerceptronClassifier::train( const DataSet& data ) {
  // Create vocab from all documents.
  m_vocab.clear();
  m_numDocs = 0;
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    addVocab( data.m_docs[i], m_vocab );
    m_numDocs += data.m_docs[i].size();
  }
  m_numAttrs = m_vocab.size(); 

  // Create a mapping of token to attribute column index for the data matrix.
  createVocabToVectorMap();

  // Create zero filled document vocab matrix, weight, d-weight, and pr vectors.
  // m_numDocs rows x numattrs+2 columns
  m_data.resize( m_numDocs, vector<int>(m_numAttrs+2,0) ); // +1 for class col, +1 for w0
  m_w.resize( m_numAttrs, 0 );
  m_error.resize( m_numAttrs, 0 );

  // Copy class labels from data.
  m_classes.clear();
  for ( unsigned int i = 0; i < data.m_classes.size(); i++ ) {
    m_classes.push_back( data.m_classes[i].first );
  } 

  // Read document vocabulary into the docVocab matrix.
  Dictionary currDocVocab;
  unsigned int k = 0;
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    for ( unsigned int j = 0; j < data.m_docs[i].size(); j++, k++ ) {
      currDocVocab.clear();
      addVocab( data.m_docs[i][j], currDocVocab );
      for ( Dictionary::iterator it = currDocVocab.begin(); it != currDocVocab.end(); it++ ) {
        string token = it->first;
        unsigned int count = it->second;
        unsigned int index = m_vocabToVector[token];
        m_data[k][index+1] = count;  // Assign value to attr column
        m_data[k][m_numAttrs+1] = atoi(m_classes[i].c_str());
        m_data[k][0] = (unsigned int)w_0; // Assign weight of w0
      }
    }
  }
 
  updateWeights();

  /*for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
    cout << m_w[i] << " ";
  }
  cout << endl;*/
}

void PerceptronClassifier::createVocabToVectorMap() {
  unsigned int index = 0;
  for ( Dictionary::iterator it = m_vocab.begin(); it != m_vocab.end(); it++, index++ ) {
    m_vocabToVector[it->first] = index;
  }
}

void PerceptronClassifier::updateWeights() {
  for ( unsigned int i = 0; i < weightUpdateIterationsLimit; i++ ) {
    for ( unsigned int j = 0; j < m_numDocs; j++ ) {
      double currDocClass = m_data[j][m_numAttrs+1];
      double currDocClassification = ((w_0+dot(m_w, m_data[j])) > 0.0 ? 1.0 : -1.0);

      // Update each weight
      for ( unsigned int k = 0; k < m_numAttrs; k++ ) {
        m_w[k] += (eta * (currDocClass - currDocClassification) * m_data[j][k+1]);
      }
    }
  }
}

double PerceptronClassifier::dot( const vector<double>& v, 
                                  const vector<int>& u )
{
  double sum = 0.0;
  // takes w and doc vocab, remember offset!
  for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
    sum += (v[i] * u[i+1]);
  }
  return sum;
}

float PerceptronClassifier::test( const DataSet &data ) {
  unsigned int correctClassifications = 0;
  int classification = 0;

  // Count number of documents total.
  unsigned int numDocs = 0;
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    numDocs += data.m_docs[i].size();
  }

  // Classify each document, track the number of correct classifications.
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    for ( unsigned int j = 0; j < data.m_docs[i].size(); j++ ) {
      classification = classify( data.m_docs[i][j] );
      int actualClass = atoi(data.m_classes[i].first.c_str());   
      if ( classification == actualClass ) {
        correctClassifications++;
      }
    }
  }

  return (float)correctClassifications / (float)numDocs;
}

int PerceptronClassifier::classify( const Document& doc ) {
  // Create document vocabulary map.
  Dictionary docVocab;
  addVocab( doc, docVocab );
  Dictionary::iterator dvit;
  Dictionary::iterator vtvit;

  double sum = w_0;

  for ( dvit = docVocab.begin(); dvit != docVocab.end(); dvit++ ) {
    vtvit = m_vocabToVector.find( dvit->first );
    int wIdx = ( vtvit == m_vocabToVector.end() ? -1 : vtvit->second );
  
    if ( wIdx != -1 ) {
      sum += m_w[wIdx] * dvit->second;
    }
  }
  return (sum > 0 ? 1 : -1); // 0 = notspam, 1 = spam
}
