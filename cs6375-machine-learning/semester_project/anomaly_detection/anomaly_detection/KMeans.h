#ifndef KMEANS_H
#define KMEANS_H

#include <deque>
#include <iostream>
#include "useful_things.h"
#include "RNG.h"

using namespace std;

class KMeans {
protected:
  unsigned int  m_k;  // Number of clusters
  deque<double> m_c;  // Cluster centers
  deque<double> m_max;// Maximum distance to any point in a cluster

  // Test if m_c == m_pc
  bool converged( const deque<unsigned int>& d1, const deque<unsigned int>& d2 ); 

  // Test if v exists in d
  bool exists( double v, const deque<double>& d );

public:
  KMeans();
  ~KMeans();

  void          setNumClusters( unsigned int k );
  unsigned int  getNumClusters() const;
  void          setCenters( const deque<double>& c );
  deque<double> getCenters() const;
  void          setMaxDistances( const deque<double>& d );
  deque<double> getMaxDistances() const;

  // populate the KMeans classifier for 1D data
  void populate( const deque<double>& data );

  // Assign x to a cluster and return cluster number
  unsigned int assignID( double x ) const;

  // Assign x to a cluster and return cluster center
  double assignVal( double x ) const;

  void print() const;

  void clearData();
};

#endif