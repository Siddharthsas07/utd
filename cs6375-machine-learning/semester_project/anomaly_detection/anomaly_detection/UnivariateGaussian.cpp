#include "UnivariateGaussian.h"

UnivariateGaussian::UnivariateGaussian() : m_u(0.0), m_v(0.0) {}

UnivariateGaussian::~UnivariateGaussian() {}

void UnivariateGaussian::setMean( double u ) {
  m_u = u;
}

double UnivariateGaussian::getMean() const {
  return m_u;
}

void UnivariateGaussian::setVariance( double u ) {
  m_v = u;
}

double UnivariateGaussian::getVariance() const {
  return m_v;
}

void UnivariateGaussian::populate( const deque<double>& data ) {
  if ( data.empty() ) {
    return;
  }

  clearData();

  // Calculate the mean of the data
  for ( unsigned int i = 0; i < data.size(); i++ ) {
    m_u += data[i];
  }
  m_u /= static_cast<double>( data.size() );

  // Calculate the variance of the data
  double diff = 0.0;
  for ( unsigned int i = 0; i < data.size(); i++ ) {
    diff = data[i] - m_u;
    m_v += diff * diff;
  }
  m_v /= static_cast<double>( data.size() );
}

double UnivariateGaussian::sample( double x ) const {
  double diff = (x - m_u);
  double numer = exp( -diff*diff / (2.0*m_v) );
  double denom = sqrt(m_v) * sqrt(2*nums::pi);
  double result = numer / denom;
  if ( result < 10E-300 ) {
    result = 10E-300;
  }
  return result;
}

void UnivariateGaussian::print() const {
  cout.precision(6);
  cout << "u: " << m_u << endl;
  cout << "v: " << m_v << endl;
}

void UnivariateGaussian::clearData() {
  m_u = 0.0;
  m_v = 0.0;
}