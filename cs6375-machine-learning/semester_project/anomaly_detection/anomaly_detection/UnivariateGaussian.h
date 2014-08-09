#ifndef UNIVARIATE_GAUSSIAN_H
#define UNIVARIATE_GAUSSIAN_H

#include <deque>
#include <iostream>
#include <math.h>
#include "useful_things.h"

using namespace std;

class UnivariateGaussian {
protected:
  double m_u;  // Mu: mean
  double m_v;  // Sigma^2: variance

public:
  UnivariateGaussian();
  ~UnivariateGaussian();

  void   setMean( double u );
  double getMean() const;
  void   setVariance( double v );
  double getVariance() const;

  void populate( const deque<double>& data );
  // calculate the mean and the variance

  double sample( double x ) const; // Use x in the sampling formula to get p(x)

  void print() const; // Print out the detector's information

  void clearData();
};

#endif