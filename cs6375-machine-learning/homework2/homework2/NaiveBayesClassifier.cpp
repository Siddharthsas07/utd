#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "NaiveBayesClassifier.h"
#include "stringHelp.h"

void NaiveBayesClassifier::train( const DataSet& data ) {
  // Add new vocab for all classes of documents in data.
  m_vocab.clear();
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    addVocab( data.m_docs[i], m_vocab );
  }

  // Copy class labels from data.
  m_classes.clear();
  for ( unsigned int i = 0; i < data.m_classes.size(); i++ ) {
    m_classes.push_back( data.m_classes[i].first );
  }

  // Count number of documents total.
  unsigned int numDocs = 0;
  for ( unsigned int i = 0; i < data.m_docs.size(); i++ ) {
    numDocs += data.m_docs[i].size();
  }

  m_prior.clear();
  m_classVocab.clear();
  for ( unsigned int i = 0; i < m_classes.size(); i++ ) {
    unsigned int numDocsThisClass = data.m_docs[i].size();    
    float prior = (float)numDocsThisClass / (float)numDocs;
    m_prior.push_back( prior );
    
    // Create class specific vocab.
    m_classVocab.resize( m_classVocab.size()+1 );   
    addVocab( data.m_docs[i], m_classVocab[i] );

    // Denominator for conditional probability update.
    float denom = (float)sumTokens(m_classVocab[i]) + (float)m_vocab.size();

    Dictionary::iterator vocabIt;
    Dictionary::iterator tokenLoc;   

    for ( vocabIt = m_vocab.begin(); vocabIt != m_vocab.end(); vocabIt++ ) {
      // Get count of global token in class specific vocab.
      string token = vocabIt->first;
      tokenLoc = m_classVocab[i].find( token );
      unsigned int tokenCount = (tokenLoc == m_classVocab[i].end()) ? 0 : tokenLoc->second;

      // Numerator for conditional probability update.
      float numer = (float)tokenCount + 1.0f;      

      // Conditional probability entry does not yet exist.
      if ( i == 0 ) {
        m_condProb[token] = vector<float>( m_classes.size(), 0.0f );
      }
      m_condProb[token][i] = (numer / denom);
    }
  }
}


float NaiveBayesClassifier::test( const DataSet &data ) {
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

unsigned int NaiveBayesClassifier::classify( const Document& doc ) {
  // Create document vocabulary map.
  Dictionary docVocab;
  addVocab( doc, docVocab );

  // Calculate score for each class.
  vector<float> score( m_classes.size(), 0.0f );
  for ( unsigned int i = 0; i < m_classes.size(); i++ ) {
    score[i] = log(m_prior[i]);

    Dictionary::iterator vocabIt;
    map<string, vector<float> >::iterator condProbLoc;

    // Iterate over all tokens in the document vocabulary and update scores.
    for ( vocabIt = docVocab.begin(); vocabIt != docVocab.end(); vocabIt++ ) {
      string token = vocabIt->first;
      condProbLoc = m_condProb.find( token );
      score[i] += ( condProbLoc == m_condProb.end() ? 0.0f : log(condProbLoc->second[i]) );
    }
  }

  // Determine which class has maximum score.
  unsigned int maxClass = 0;
  float maxScore = (float)INT_MIN;
  for ( unsigned int i = 0; i < m_classes.size(); i++ ) {
    if ( score[i] > maxScore ) {
      maxClass = i;
      maxScore = score[i];
    }
  }

  return maxClass;
}
