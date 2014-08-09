#ifndef MULTIVARIATE_GAUSSIAN_H
#define MULTIVARIATE_GAUSSIAN_H

#include <deque>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "useful_things.h"

using namespace std;

class MultivariateGaussian {
protected:
  deque<double>         m_u;  // Mu: Means deque
  deque<deque<double> > m_s;  // Sigma: Covariance matrix
  deque<deque<double> > m_si; // Sigma inverse, needed for sampling
  double                m_sd; // Sigma determinant, needed for sampling

  // Return minor of M with row and column removed
  deque<deque<double> > getMinorMatrix( const deque<deque<double> >& M, unsigned int row, unsigned int col ) const;

  // r = M*v
  deque<double> mult( const deque<deque<double> >& M, const deque<double>& v ) const;

  // r = u*v; pairwise multiplication
  deque<double> mult( const deque<double>& u, const deque<double>& v ) const;

  // r = f*M
  deque<deque<double> > mult( double f, const deque<deque<double> >& M ) const;

  // r = u-v
  deque<double> minus( const deque<double>& u, const deque<double>& v ) const;

  // r = u.v
  double dot( const deque<double>& u, const deque<double>& v ) const;

  // Laplace's algorithm to calculate determinant
  double laplaceDet( const deque<deque<double> >& M ) const;

  // Covariance of columns i and j in data
  double cov( unsigned int i, unsigned int j, const deque<deque<double> >& data, const deque<double>& means );

public:
  MultivariateGaussian();
  ~MultivariateGaussian();

  void                  setMean( const deque<double>& u );
  deque<double>         getMean() const;
  void                  setCovarianceMatrix( const deque<deque<double> >& s );
  deque<deque<double> > getCovarianceMatrix() const;
  void                  setCovarianceMatrixInverse( const deque<deque<double> >& s );
  deque<deque<double> > getCovarianceMatrixInverse() const;
  void                  setCovarianceMatrixDeterminant( double d );
  double                getCovarianceMatrixDeterminant() const;

  // |M|, M = nxn
  double det( const deque<deque<double> >& M ) const;

  // lhs = rhs^-1
  deque<deque<double> > invert( const deque<deque<double> >& M ) const;  

  void populate( const deque<deque<double> >& data );

  double sample( const deque<double>& x ) const;

  void print() const;

  void clearData();
};

#endif