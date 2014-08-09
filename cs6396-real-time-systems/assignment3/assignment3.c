#include "dodrio.h"
#include "scheduler.h"

#include <pob-eye.h>

int  main(void) {  
  //Initialize the robit
  initRobit();  
  
  // Approach the track
  approachTrack(); 

  // Run the schedule 
  scheduler();
 
  return 0;  
}
