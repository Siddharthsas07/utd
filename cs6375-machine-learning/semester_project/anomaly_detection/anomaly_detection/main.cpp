#include "useful_things.h"
#include <iostream>
#ifdef WINDOWS
  #include "hr_timer.h"
#endif

#include "AnomalyDetector.h"

using namespace std;

void main( int argc, char* argv[] ) {
  // Capture command line parameters
  if ( argc != 3 ) {
    cout << "Incorrect command line parameters. See readme for proper usage." << endl;
    return;
  }

  AnomalyDetector ad;  
  //ad.setAlg( UNIVARIATE_GAUSSIAN );
  //ad.setAlg( MULTIVARIATE_GAUSSIAN );
  ad.setAlg( KMEANS );
  ad.setCVPercent( .85 );
  ad.setMode( UNSUPERVISED );
  ad.setNumFolds( 10 );

  // Read data in 
  if ( strcmp(argv[1], "1") == 0 ) {
    ad.readData( argv[2] );
  }
  
  ad.train();
  
  ad.test();

  ad.print();

  return;
}
