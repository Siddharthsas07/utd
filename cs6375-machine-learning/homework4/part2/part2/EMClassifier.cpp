#include "EMClassifer.h"

EMClassifier::EMClassifier() {
  m_k = 0;
  m_constVariance = false;
}

EMClassifier::~EMClassifier() {}

// Clears all the data held by the classifier
void EMClassifier::clearData() {
  m_d.clear();
  m_cp.clear();
  m_p.clear();
  m_pnew.clear();
  m_u.clear();
  m_s.clear();
}

// Assumes k has already been set, allocates array space to work with.
void EMClassifier::allocateSpace() {
  double initial_p = 1.0 / m_k;

  m_cp.assign( m_d.size(), vector<double>(m_k, 0.0) );

  m_p.resize( m_k );
  m_p[0] = .25; m_p[1] = .5; m_p[2] = .25;
  m_pnew = m_p;

  m_u.resize( m_k );
  m_u[0] = 5; m_u[1] = 15; m_u[2] = 25;

  m_s.resize( m_k );
  if ( !m_constVariance ) {
    m_s[0] = 0.2; m_s[1] = 1.0; m_s[2] = 2.5;
  }  
  else {
    m_s[0] = m_variance; m_s[1] = m_variance; m_s[2] = m_variance;
  }
}

void EMClassifier::setNumClusters( unsigned int k ) {
  m_k = k;
}

unsigned int EMClassifier::getNumClusters() {
  return m_k;
}

// Send in a constant variance to use (ie 1.0) or a negative value to disable
// constant variance and let the algorithm compute it.
void EMClassifier::setVariance( double var ) {
  m_variance = var;
  m_constVariance = m_variance < 0.0 ? false : true;
}

// Only makes sense if you've used setVariance for a positive value
double EMClassifier::getVariance() {
  return m_variance;
}

// Read the 1D data stored in the file pointed to by file
void EMClassifier::read( const string& file ) {
  try {
    string line;
 
    // Load class index file and data locations.
    ifstream inFile( file.c_str() );    
    if ( inFile.is_open() ) {
      clearData();
     
      while ( inFile.good() ) {
        getline( inFile, line );
        if ( line != "" && line != "\n" ) {
          m_d.push_back( strtod( line.c_str(), NULL ) );
        }
      }

      inFile.close();
    } 
  }

  catch ( exception& e ) {
    cout << e.what() << endl;
  }
}

// Print out the values.
void EMClassifier::print() {
  cout << "Means: " << endl;
  for ( unsigned int i = 0; i < m_u.size(); i++ ) {
    cout << m_u[i] << endl;
  }

  cout << "Variances: " << endl;
  for ( unsigned int i = 0; i < m_s.size(); i++ ) {
    cout << m_s[i] << endl;
  }

  cout << "Probability Totals Per Cluster: " << endl;
  for ( unsigned int i = 0; i < m_p.size(); i++ ) {
    cout << m_pnew[i] << endl;
  }

}

// Just a univariate gaussian because the data is 1D
double EMClassifier::gaussian( unsigned int dataIndex, unsigned int meanIndex, unsigned int varianceIndex ) {
  double meanDiff = m_d[dataIndex] - m_u[meanIndex];

  double numer = exp( (-1.0 * meanDiff * meanDiff) / (2.0 * m_s[varianceIndex]) );

  double denom = sqrt(2 * nums::pi) * sqrt(m_s[varianceIndex]); 

  double result = numer / denom;

  if ( result < 10E-100 ) {
    return 10E-100;
  }

  return result;
}

bool EMClassifier::converged() {
  double diff = 0.0;

  for ( unsigned int i = 0; i < m_p.size(); i++ ) {
    diff += fabs( m_p[i] - m_pnew[i] );
    if ( diff > 1E-6 ) {
      return false;
    }
  }

  return true;
}

void EMClassifier::normalize( vector<double>& v ) {
  double magnitude = 0.0;
  for ( unsigned int i = 0; i < v.size(); i++ ) {
    magnitude += (v[i]*v[i]);
  }
  magnitude = sqrt(magnitude);
  for ( unsigned int i = 0; i < v.size(); i++ ) {
    if ( magnitude == 0.0 ) {
      v[i] = 0;
    }
    else {
      v[i] /= magnitude;
    }
  }
}

// Perform EM Algorithm
void EMClassifier::train() {
  allocateSpace();

  unsigned int iterations = 0;
  do {
    m_p = m_pnew;

    E_Step();

    M_Step();
  } while( !converged() );

  print();
}

// Update probabilities of belonging to each class for each data point. Normalize.
void EMClassifier::E_Step() {
  for ( unsigned int i = 0; i < m_k; i++ ) {
    for ( unsigned int j = 0; j < m_d.size(); j++ ) {
      m_cp[j][i] = m_p[i] * gaussian( j, i, i );
    }
  }

  for ( unsigned int i = 0; i < m_cp.size(); i++ ) {
    normalize( m_cp[i] );
  }
}

// Update means for clusters, covariance matrix entries, and total probabilities for each class.
void EMClassifier::M_Step() {
  vector<double> cpsum( m_k, 0.0 );

  // Pre compute class probability totals for each class
  for ( unsigned int i = 0; i < m_k; i++ ) {
    for ( unsigned int j = 0; j < m_d.size(); j++ ) {
      cpsum[i] += m_cp[j][i];
    }
  }

  double temp = 0.0;

  // Update u's
  for ( unsigned int i = 0; i < m_k; i++ ) {
    temp = 0.0;
    for ( unsigned int j = 0; j < m_d.size(); j++ ) {
      temp += m_cp[j][i] * m_d[j];
    } 
    m_u[i] = temp / cpsum[i];
  }

  // Update s's (only if a constant variance hasn't been specified)
  if ( !m_constVariance ) {
    for ( unsigned int i = 0; i < m_k; i++ ) {
      temp = 0.0;
      for ( unsigned int j = 0; j < m_d.size(); j++ ) {
        double meanDiff = m_d[j] - m_u[i];
        temp += m_cp[j][i] * meanDiff * meanDiff;
      }
      m_s[i] = temp / cpsum[i];
    }
  }

  // Update p's
  for ( unsigned int i = 0; i < m_k; i++ ) {
    m_pnew[i] = cpsum[i] / m_d.size();
  }
  normalize( m_pnew );
}