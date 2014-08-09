#include "KMeans.h"

KMeans::KMeans() : m_k(0) {}

KMeans::~KMeans() {}

void KMeans::setNumClusters( unsigned int k ) {
  m_k = k;
}

unsigned int KMeans::getNumClusters() const {
  return m_k;
}

void KMeans::setCenters( const deque<double>& c ) {
  m_c = c;
}

deque<double> KMeans::getCenters() const {
  return m_c;
}

void KMeans::setMaxDistances( const deque<double>& d ) {
  m_max = d;
}

deque<double> KMeans::getMaxDistances() const {
  return m_max;
}

void KMeans::populate( const deque<double>& data ) {
  if ( m_k == 0 ) {
    cout << "Cannot train with 0 clusters." << endl;
    return;
  }

  m_c.resize( m_k );

  deque<unsigned int> clusterAssignment( data.size(), 0 );
  deque<unsigned int> prevClusterAssignment( data.size(), 0 );

  double maxDist = 0.0;
  double curDist = 0.0;
  unsigned int closestCenter = 0;

  // Seed initial cluster centers by picking k random values from the supplied
  // set. Do not allow repeats.
  RNG<int> rng;
  double centerValue = 0.0;
  for ( unsigned int i = 0; i < m_c.size(); i++ ) {
    do {
      centerValue = data[rng.rand(data.size())];
    } while ( exists( centerValue, m_c ) );
    m_c[i] = centerValue;
  } 

  // Kmeans
  do {
    // Record previous centers for testing convergence later
    prevClusterAssignment = clusterAssignment;

    // For each data point find its closest cluster center, update cluster assignment
    for ( unsigned int i = 0; i < data.size(); i++ ) {
      maxDist = DBL_MAX;
      
      for ( unsigned int j = 0; j < m_c.size(); j++ ) {
        curDist = fun::makeFinite( fun::dist( data[i], m_c[j] ) );
        if ( curDist < maxDist ) {
          maxDist = curDist;
          closestCenter = j;
        }
      }

      clusterAssignment[i] = closestCenter;
    }

    // Update cluster centers as running averages of assigned points
    deque<unsigned int> timesAssignedToCluster( m_k, 0 );
    
    for ( unsigned int i = 0; i < data.size(); i++ ) {
      double a = ++timesAssignedToCluster[clusterAssignment[i]];
      // center = center * proportion + data * proportion
      m_c[clusterAssignment[i]] = fun::makeFinite( (m_c[clusterAssignment[i]] * ((a - 1.0) / a ))
                                                 + (data[i] / a) );
    }
  } while ( !converged( clusterAssignment, prevClusterAssignment ) );
  
  // Calculate max distances
  m_max.resize( m_k, 0.0 );
  double dist = 0.0;

  // Compute max distance to assigned cluster's center
  for ( unsigned int i = 0; i < data.size(); i++ ) {
    dist = fun::makeFinite( fun::dist(data[i], m_c[clusterAssignment[i]]) );

    if ( dist > m_max[clusterAssignment[i]] ) {
      m_max[clusterAssignment[i]] = dist;
    }
  }
}

unsigned int KMeans::assignID( double x ) const {
  double currMinDistance = DBL_MAX;
  unsigned int currAssignment = 0;
  double distance = 0.0;

  for ( unsigned int i = 0; i < m_c.size(); i++ ) {
    distance = fun::makeFinite( fun::dist( x, m_c[i]) );
    if ( distance < currMinDistance ) {
      currMinDistance = distance;
      currAssignment = i;
    }
  }

  return currAssignment;
}

double KMeans::assignVal( double x ) const {
  unsigned int center = assignID( x );
  return m_c[center];
}

bool KMeans::converged( const deque<unsigned int>& d1, const deque<unsigned int>& d2 ) {
  for ( unsigned int i = 0; i < d1.size(); i++ ) {
    if ( d1[i] != d1[i] ) {
      return false;
    }
  }
  return true;
}

bool KMeans::exists( double v, const deque<double>& d ) {
  for ( unsigned int i = 0; i < d.size(); i++ ) {
    if ( d[i] == v ) {
      return true;
    }
  }
  return false;
}

void KMeans::print() const {
  cout.precision(6);  
  cout << "n: " << m_k << endl;
  cout << "c:" << endl;  
  for ( unsigned int i = 0; i < m_c.size(); i++ ) {
    cout << m_c[i] << endl;
  }
  cout << "d:" << endl;
  for ( unsigned int i = 0; i < m_max.size(); i++ ) {
    cout << m_max[i] << endl;
  }
}

void KMeans::clearData() {
  m_c.clear();
  m_max.clear();
}