#include <iostream>
#include "Converter.h"
#include "useful_things.h"
#ifdef WINDOWS
  #include "hr_timer.h"
#endif

using namespace std;

void main( int argc, char* argv[] ) {
  // Capture command line parameters
  if ( argc != 4 && argc != 5 ) {
    cout << "Incorrect command line parameters. See readme for proper usage." << endl;
    return;
  }

  #ifdef WINDOWS
    CStopWatch timer;
    timer.startTimer();
  #endif

  if ( argc == 4 ) {
    Converter converter( argv[1], argv[2], argv[3], "\n -:;!@#$%^&*()=+[]{}<>,.?/\\\"'|", 3 );
    converter.outputARFF( argv[1] );
  }
  else if ( argc == 5 ) {
    Converter converter( argv[1], argv[2], argv[3], argv[4], "\n -:;!@#$%^&*()=+[]{}<>,.?/\\\"'|", 3 );
    converter.outputARFF( argv[1] );
  }
  
  #ifdef WINDOWS
    timer.stopTimer();
    cout << "Data converted in " << timer.getElapsedTime() << endl;
  #endif
}
