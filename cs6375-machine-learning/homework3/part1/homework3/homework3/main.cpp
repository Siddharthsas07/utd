#include "useful_things.h"
#include <iostream>
#ifdef WINDOWS
  #include "hr_timer.h"
#endif

#include "DataSet.h"
#include "PerceptronClassifier.h"

#include <cmath>
#include <cfloat>

using namespace std;

void main( int argc, char* argv[] ) {
  // Capture command line parameters
  if ( argc != 4 ) {
    cout << "Incorrect command line parameters. See readme for proper usage." << endl;
    return;
  }

#ifdef WINDOWS
  CStopWatch timer;
#endif

#ifdef WINDOWS
  timer.startTimer();
#endif
  DataSet trainingData;
  if ( !trainingData.loadData( argv[1], argv[2] ) ) {
    cout << "Could not load training data." << endl;
  }
#ifdef WINDOWS
  timer.stopTimer();
  cout << "Loading training data completed in " << timer.getElapsedTime() << endl;
#endif

#ifdef WINDOWS
  timer.startTimer();
#endif
  DataSet testData;
  if ( !testData.loadData( argv[1], argv[3] ) ) {
    cout << "Could not load test data." << endl;
  }
#ifdef WINDOWS
  timer.stopTimer();
  cout << "Loading test data completed in " << timer.getElapsedTime() << endl;
#endif

#ifdef WINDOWS
  timer.startTimer();
#endif
  PerceptronClassifier PC;
  PC.train( trainingData );
  cout << "Perceptron accuracy = " << PC.test( testData ) << endl;
#ifdef WINDOWS
  timer.stopTimer();
  cout << "Perceptron completed in " << timer.getElapsedTime() << endl;
#endif
  
  return;
}
