#include "Controller.h" 

//const unsigned int PATH_LENGTH = 1; 
//Matrix references[PATH_LENGTH]; // TODO
Controller controller;

setup() {
  // Seed references with position 5 inches up
}

loop() {
  controller.start();

  // While we haven't sent a kill signal to the controller
  while ( controller.get_run_state() == CONT_RUN ) {
    // For each reference make the controller transition to that reference state
    for ( unsigned int i = 0; i < PATH_LENGTH; i++ ) {
      controller.apply( references[i] );
    }
  }
  
  if ( controller.get_run_state() == CONT_ERROR ) {
    exit(1);
  }
}
