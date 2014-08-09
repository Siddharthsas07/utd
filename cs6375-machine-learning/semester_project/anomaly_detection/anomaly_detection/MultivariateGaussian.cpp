#include "MultivariateGaussian.h"

MultivariateGaussian::MultivariateGaussian() : m_sd(0.0) {}

MultivariateGaussian::~MultivariateGaussian() {}

void MultivariateGaussian::setMean( const deque<double>& u ) {
  m_u = u;
}

deque<double> MultivariateGaussian::getMean() const {
  return m_u;
}

void MultivariateGaussian::setCovarianceMatrix( const deque<deque<double> >& s ) {
  m_s = s;
}

deque<deque<double> > MultivariateGaussian::getCovarianceMatrix() const {
  return m_s;
}

void MultivariateGaussian::setCovarianceMatrixInverse( const deque<deque<double> >& s ) {
  m_si = s;
}

deque<deque<double> > MultivariateGaussian::getCovarianceMatrixInverse() const {
  return m_si;
}

void MultivariateGaussian::setCovarianceMatrixDeterminant( double d ) {
  m_sd = d;
}

double MultivariateGaussian::getCovarianceMatrixDeterminant() const {
  return m_sd;
}

void MultivariateGaussian::populate( const deque<deque<double> >& data ) {
  if ( data.empty() ) {
    return;
  }

  clearData();
  // Calculate the means for each column.
  double sum = 0.0;
  for ( unsigned int j = 0; j < data[0].size(); j++ ) {
    sum = 0.0;

    for ( unsigned int i = 0; i < data.size(); i++ ) {
      sum += data[i][j];
    }

    m_u.push_back( sum / static_cast<double>( data.size() ) );
  }

  // Calculate the covariance matrix.
  m_s.resize( m_u.size() );
  for ( unsigned int i = 0; i < m_s.size(); i++ ) {
    m_s[i].resize( m_u.size() );
  }

  for ( unsigned int i = 0; i < m_s.size(); i++ ) {
    for ( unsigned int j = 0; j < m_s[i].size(); j++ ) {
      m_s[i][j] = cov( i, j, data, m_u );
    }
  }

  // Calculate and store the inverse of the covariance matrix
  m_si = invert( m_s );

  // Calculate and store the determinant of the covariance matrix
  m_sd = det( m_s );
}

double MultivariateGaussian::sample( const deque<double>& x ) const {
  deque<double> diff = minus( x, m_u );

  double numer = exp( -0.5 * dot( diff, mult( m_si, diff ) ) );
  double denom = pow( (2*nums::pi), m_u.size() / 2.0 ) * sqrt(m_sd);

  if ( denom == 0.0 ) {
    return 0.0; 
  }

  double result = numer / denom;

  if ( result < 10E-300 ) {
    result = 10E-300;
  }
  else if ( result > 10E300 ) {
    result = 10E300;
  }
  return result;
}

void MultivariateGaussian::print() const {
  cout.precision(6);  
  cout << "u:" << endl;
  for ( unsigned int i = 0; i < m_u.size(); i++ ) {
    cout << m_u[i] << endl;
  }

  cout << endl;
  cout << "s:" << endl;
  cout.width(10);
  cout.fill(' ');

  for ( unsigned int i = 0; i < m_s.size(); i++ ) {
    for ( unsigned int j = 0; j < m_s[i].size(); j++ ) {
      cout << setw(10) << m_s[i][j];
    }
    cout << endl;
  }
}

deque<deque<double> > MultivariateGaussian::invert( const deque<deque<double> >& M ) const {
  if ( M.size() == 3 ) {
    double A, B, C, D, E, F, G, H, K, a, b, c, d, e, f, g, h, k;
    a = M[0][0]; b = M[0][1]; c = M[1][1];
    d = M[1][0]; e = M[1][1]; f = M[1][2];
    g = M[2][0]; h = M[2][1]; k = M[2][2];

    A = e*k - f*h; D = c*h - b*k; G = b*f - c*e;
    B = f*g - d*k; E = a*k - c*g; H = c*d - a*f;
    C = d*h - e*g; F = g*b - a*h; K = a*e - b*d;

    deque<deque<double> > r(M);
    r[0][0] = A; r[0][1] = D; r[0][2] = G;
    r[1][0] = B; r[1][1] = E; r[1][2] = H;
    r[2][0] = C; r[2][1] = F; r[2][2] = K;

    return mult( 1.0/det(M), r );
  }
  else if ( M.size() == 2 ) {
    deque<deque<double> > r(M);
    r[0][0] = M[1][1];
    r[0][1] = -M[0][1];
    r[1][0] = -M[1][0];
    r[1][1] = M[0][0];

    return mult( 1.0/det(M), r );
  }
  else if ( M.size() == 1 ) {
    deque<deque<double> > r(M);
    r[0][0] = 1.0 / r[0][0];
    return r;
  }
  else if ( M.size() == 0 ) {
    return M;
  }
  else {
    deque<deque<double> > r(M);
    deque<deque<double> > minor;

    double scalar = 1.0 / det( M );

    // Assumes sqare matrix
    for ( unsigned int j = 0; j < M.size(); j++ ) {
      for ( unsigned int i = 0; i < M.size(); i++ ) {
        r[i][j] = scalar * det( getMinorMatrix( M, j, i ) );
        if ( ((i + j) % 2) == 1 ) {
          r[i][j] = -r[i][j];
        }
      }
    }

    return r;
  }  
}

deque<deque<double> > MultivariateGaussian::getMinorMatrix( const deque<deque<double> >& M, unsigned int row, unsigned int col ) const {
  if ( M.empty() ) {
    return M;
  }

  deque<deque<double> > r(M);

  // Remove the requested row
  r.erase( r.begin() + row );

  // For each row, delete jth entry.
  for ( unsigned int i = 0; i < r.size(); i++ ) {
    r[i].erase( r[i].begin()+col );
  }

  return r;
}

deque<double> MultivariateGaussian::mult( const deque<deque<double> >& M, const deque<double>& v ) const {
  deque<double> r( M.size() );
  for ( unsigned int i = 0; i < r.size(); i++ ) {
    r[i] = dot( M[i], v );
  }
  return r;
}

deque<double> MultivariateGaussian::mult( const deque<double>& u, const deque<double>& v ) const {
  deque<double> r( u.size() );
  for ( unsigned int i = 0; i < r.size(); i++ ) {
    r[i] = (u[i] * v[i]);
  }
  return r;
}

deque<deque<double> > MultivariateGaussian:: mult( double f, const deque<deque<double> >& M ) const {
  deque<deque<double> > r(M);
  for ( unsigned int i = 0; i < r.size(); i++ ) {
    for ( unsigned int j = 0; j < r[i].size(); j++ ) {
      r[i][j] *= f;
    }
  }
  return r;
}

deque<double> MultivariateGaussian::minus( const deque<double>& u, const deque<double>& v ) const {
  deque<double> r( u.size() );
  for ( unsigned int i = 0; i < r.size(); i++ ) {
    r[i] = (u[i] - v[i]);
  }
  return r;
}

double MultivariateGaussian::dot( const deque<double>& u, const deque<double>& v ) const {
  double sum = 0.0;
  for ( unsigned int i = 0; i < u.size(); i++ ) {
    sum += (u[i] * v[i]);
  }
  return sum;
}

double MultivariateGaussian::det( const deque<deque<double> >& M ) const {
  return laplaceDet( M );
}

double MultivariateGaussian::laplaceDet( const deque<deque<double> >& M ) const {
  if ( M.size() == 3 ) {
    return ( M[0][0] * ( M[1][1] * M[2][2] - M[1][2] * M[2][1] )
           + M[0][1] * ( M[1][2] * M[2][0] - M[2][2] * M[1][0] )
           + M[0][2] * ( M[1][0] * M[2][1] - M[1][1] * M[2][0] ) );
  }
  else if ( M.size() == 2 ) {
    return ( (M[0][0] * M[1][1]) - (M[0][1] * M[1][0]) );
  }
  else if ( M.size() == 1 ) {
    return M[0][0];
  }
  else if ( M.size() == 0 ) {
    return 0.0;
  }
  else {
    double sum = 0.0;
    double weight = 0.0;

    deque<deque<double> > minor;

    for ( unsigned int i = 0; i < M.size(); i++ ) {
      minor = getMinorMatrix( M, 0, i );

      sum += ( (i % 2 == 1) ? -1.0 : 1.0 ) * M[0][i] * laplaceDet( minor );
    }

    return sum;
  }
}

double MultivariateGaussian::cov( unsigned int X, unsigned int Y, const deque<deque<double> >& data, const deque<double>& means ) {
  if ( data.empty() ) {
    return 0.0;
  }

  double sum = 0.0;

  for ( unsigned int i = 0; i < data.size(); i++ ) { 
    sum += ( data[i][X] - m_u[X] ) * ( data[i][Y] - m_u[Y] );
  }

  double result = sum / static_cast<double>( data.size() );
  return result;
}

void MultivariateGaussian::clearData() {
  m_u.clear();
  m_s.clear();
  m_si.clear();
  m_sd = 0.0;
}