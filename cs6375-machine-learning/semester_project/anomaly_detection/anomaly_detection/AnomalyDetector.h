#ifndef ANOMALY_DETECTOR_H
#define ANOMALY_DETECTOR_H

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "useful_things.h"
#include "MultivariateGaussian.h"
#include "UnivariateGaussian.h" 
#include "KMeans.h"
#ifdef WINDOWS
  #include "hr_timer.h"
#endif

using namespace std;

// Supported rendering modes
enum LEARNING_ALGORITHM {
  UNIVARIATE_GAUSSIAN,
  MULTIVARIATE_GAUSSIAN,
  KMEANS
};

enum LEARNING_MODE {
  SUPERVISED,
  SEMI_SUPERVISED,
  UNSUPERVISED
};

class AnomalyDetector {
protected:
  // Data 
  deque<deque<string> > m_h;      // Headers for data matrix
  deque<deque<double> > m_d;      // Sample data matrix
  deque<deque<double> > m_ad;     // Sample anomalous data
  deque<deque<double> > m_traind; // Subset of samples from m_d for training
  deque<deque<double> > m_trainad;// Subset of samples from m_ad for training
  deque<deque<double> > m_testd;  // Subset of samples from m_d for testing
  deque<deque<double> > m_testad; // Subset of samples from m_ad for testing

  // Algorithms and learning modes
  LEARNING_ALGORITHM  m_alg;   // The algorithm for learning
  LEARNING_MODE       m_mode;  // The mode for learning
  bool                m_valid; // Whether test() will be successful or not

  // Tweak config
  double m_cvPercent;          // Percent of input data to use for cross validation.
  unsigned int m_folds;        // Number of folds to perform during training to learn boundaries

  // Models
  MultivariateGaussian      m_mg;  // one gaussian for all attributes
  deque<UnivariateGaussian> m_ug;  // one gaussian for each attribute
  double                    m_eps; // Epsilon: threshold. p(x) below this = anomalous
  double calculateEpsilon( const deque<deque<double> >& okSet, 
                           const deque<deque<double> >& anomSet ) const;

  deque<deque<KMeans> > m_k;    // one kmeans for each attribute in each set
  deque<double>         m_dist; // Maximum OK distance for each KMeans cluster
  
  double evaluate( const deque<double>& x ) const; // Get P(x) from the current model 

  // Partition a data set into two, main and rest, with mainPercent of d going to main
  void partitionDataSet( const deque<deque<double> >& d,
                         deque<deque<double> >& main,
                         deque<deque<double> >& rest,
                         double mainPercent );

  // Test okSet and anomSet on the current classifier
  double crossValidate( const deque<deque<double> >& okSet, 
                        const deque<deque<double> >& anomSet,
                        double eps ) const;

public:
  AnomalyDetector();
  ~AnomalyDetector();

  // Change the algorithm used for classification.
  // Changing the alg or mode invalidates the state. Train again to revalidate.
  void setAlg( LEARNING_ALGORITHM alg ); 
  LEARNING_ALGORITHM getAlg() const;
  
  // Change the learning mode. If unsupervised, read() will not attempt to pre-classify
  // anomalous-looking data points (things with NaN or INF). If semi-supervised
  // read() will attempt to pre-classify anomalous-looking data points. If no 
  // anomalous-data points are in the data, read sets mode to UNSUPERVISED
  void setMode( LEARNING_MODE mode ); 
  LEARNING_MODE getMode() const;

  void setCVPercent( double p ); // Clamps to [0.0, 1.0]
  double getCVPercent() const;
  void setNumFolds( unsigned int f );
  unsigned int getNumFolds() const;

  void readData( const string& file );

  // pre: readData 
  void train(); 

  // pre: train
  double test() const;  

  // pre: train
  // true = anomalous, false = OK
  bool classify( const deque<double>& x ) const;

  void print() const;

  void clearData();
};

#endif