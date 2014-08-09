#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "LogisticRegressionClassifier.h"
#include "stringHelp.h"
#include "hr_timer.h"

const unsigned int gradientAscentIterationsLimit = 50;
const double w_0 = 1.0;
const double eta = -0.1;
const double lambda = 0.1;

void LogisticRegressionClassifier::train( const DataSet& data ) {
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
  m_data.resize( m_numDocs, vector<unsigned int>(m_numAttrs+2,0) ); // +1 for class col, +1 for w0
  m_w.resize( m_numAttrs, 0.00000001 );
  m_dw.resize( m_numAttrs, 0 );
  m_pr.resize( m_numDocs, 0 );

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
        m_data[k][m_numAttrs+1] = i; // Assign class to right hand column
        m_data[k][0] = (unsigned int)w_0; // Assign weight of w0
      }
    }
  }

  // Copy class labels from data.
  m_classes.clear();
  for ( unsigned int i = 0; i < data.m_classes.size(); i++ ) {
    m_classes.push_back( data.m_classes[i].first );
  }  
 
  gradientAscent();
}

void LogisticRegressionClassifier::createVocabToVectorMap() {
  unsigned int index = 0;
  for ( Dictionary::iterator it = m_vocab.begin(); it != m_vocab.end(); it++, index++ ) {
    m_vocabToVector[it->first] = index;
  }
}

void LogisticRegressionClassifier::gradientAscent() {
  for ( unsigned int k = 0; k < gradientAscentIterationsLimit; k++ ) {
    // Precompute probabilities.
    for ( unsigned int i = 0; i < m_numDocs; i++ ) {
      m_pr[i] = pHat(i);
    }

    // zero out d-weights
    for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
      m_dw[i] = 0;
    }

    // Calculate d-weights.
    for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
      for ( unsigned int j = 0; j < m_numDocs; j++ ) {
        m_dw[i] += (m_data[j][i+1] * (m_data[j][m_numAttrs+1] - m_pr[j]));
      }
    }

    // Update weights 
    for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
      m_w[i] += eta*( -(lambda*m_w[i]) + m_dw[i]);
    }   
  }
}

double LogisticRegressionClassifier::pHat( unsigned int idx ) {
  double sum = 0;

  for ( unsigned int i = 0; i < m_numAttrs; i++ ) {
    sum += (m_w[i]*m_data[idx][i+1]);
    if ( sum > 700.0 ) { break; }
    // ln(DBL_MAX) == 709, so break if we get bigger than that to avoid overflow below
  }

  // exp(w_0+sum)/(1+exp(w_0+sum) == 1-1/(1+exp(w_0+sum)) by partial fraction decomposition
 
  //return ( sum > 708.0 ? 1.0 : (1.0 - (1.0 / (exp(w_0+sum)+1.0))) );
  return ( sum > 708.0 ? 0.0 : (1.0 / (exp(w_0+sum)+1.0)) );
}

float LogisticRegressionClassifier::test( const DataSet &data ) {
  unsigned int correctClassifications = 0;
  unsigned int classification = 0;

  // Count number of documents total.
  unsigned int numDocs = 0;
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    numDocs += data.m_docs[i].size();
  }

  // Classify each document, track the number of correct classifications.
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    for ( unsigned int j = 0; j < data.m_docs[i].size(); j++ ) {
      classification = classify( data.m_docs[i][j] );
      if ( classification == i ) {
        correctClassifications++;
      }
    }
  }

  return (float)correctClassifications / (float)numDocs;
}

unsigned int LogisticRegressionClassifier::classify( const Document& doc ) {
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

  return ( sum < -w_0 ? 1 : 0 );
}
