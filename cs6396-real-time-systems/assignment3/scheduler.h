//
//  scheduler.h
//  scheduler
//
//  Created by Melyssa Son on 3/21/13.
//
//

#ifndef scheduler_scheduler_h
#define scheduler_scheduler_h

#include "tasks.h"

#define FRAMES 4                /* Frames per hyperperiod */
#define FRAME_SIZE 10
#define MAX_TASKS_PER_FRAME 10  /* Maximum periodic tasks per frame */

extern Task lookLeftTask;
extern Task lookRightTask;
extern Task lookCenterTask;
extern Task lookLeftCenterTask;
extern Task lookRightCenterTask;
extern Task realignCenterTask;
extern Task captureImgLeftTask;
extern Task captureImgRightTask;
extern Task captureImgCenterTask;
extern Task captureImgLeftCenterTask;
extern Task captureImgRightCenterTask;
extern Task initiateCrossingTask;
extern Task crossTrackTask;
extern Task avoidRobotTask;
extern Task approachTrackTask;

/* A two dimensional array of Task pointers of the task schedule
 Each row corresponds to a frame. The row contains pointers to the
 tasks to be exectuted in the frame in order. The last task to be
 executed should be followed by NULL. Denoted by L(k) on page 91
 in the book by Liu. */
extern Task * tasks[FRAMES][MAX_TASKS_PER_FRAME];

void scheduler();

#endif
