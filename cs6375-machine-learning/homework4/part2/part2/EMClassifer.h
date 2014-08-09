#ifndef EM_CLASSIFIER_H
#define EM_CLASSIFIER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "useful_things.h"

using namespace std;

class EMClassifier {
private:
  vector<double> m_d;           // 1xN The data (1D array of doubles)
  vector<vector<double> > m_cp; // MxD Probability of each instance belonging to each class
  vector<double> m_p;           // Total probability for a class
  vector<double> m_pnew;        // Total probability for a class (after M)
  vector<double> m_u;           // MxN Mean vectors for clusters
  vector<double> m_s;           // NxN Covariance matrix for the attributes
  unsigned int m_k;             // Number of classes/clusters

  double m_variance;            // A constant variance for the gaussians, if specified by user
  bool   m_constVariance;       // Has the variance been specified as a constant?

protected:
  void clearData();
  void allocateSpace();
  double gaussian( unsigned int dataIndex, unsigned int meanIndex, unsigned int varianceIndex );
  bool converged();
  void normalize( vector<double>& v );

  void E_Step();
  void M_Step();

public:
  EMClassifier();
  ~EMClassifier();

  void setNumClusters( unsigned int k );
  unsigned int getNumClusters();

  void setVariance( double var );
  double getVariance();

  void read( const string& file );    // Only works for 1D data, one point per line

  void print();

  void train();
};

#endif