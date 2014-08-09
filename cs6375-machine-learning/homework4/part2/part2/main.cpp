#include "useful_things.h"
#include <iostream>
#ifdef WINDOWS
  #include "hr_timer.h"
#endif

#include "EMClassifer.h"

using namespace std;

void main( int argc, char* argv[] ) {
  // Capture command line parameters
  if ( argc != 2 ) {
    cout << "Incorrect command line parameters. See readme for proper usage." << endl;
    return;
  }

#ifdef WINDOWS
  CStopWatch timer;
#endif

  // Create classifier and read data
  EMClassifier em;
  em.read( argv[1] );

#ifdef WINDOWS
  timer.startTimer();
#endif

  // Train with variable variance. 
  em.setNumClusters( 3 );
  em.train();

#ifdef WINDOWS
  timer.stopTimer();
  cout << "EM training completed in " << timer.getElapsedTime() << endl;
#endif

#ifdef WINDOWS
  timer.startTimer();
#endif

  // Train with constant variance of 1.0
  em.setNumClusters( 3 );
  em.setVariance( 1.0 );
  em.train();

#ifdef WINDOWS
  timer.stopTimer();
  cout << "EM training completed in " << timer.getElapsedTime() << endl;
#endif
  
  return;
}
