//
//  tasks.h
//  scheduler
//
//  Created by Melyssa Son on 3/21/13.
//
//

#ifndef scheduler_tasks_h
#define scheduler_tasks_h

/* Task Function prototypes */

typedef struct
{
    void (*taskFunction)(void);
    int executionTime;
} Task;

void lookLeft();
void lookRight();
void lookCenter();
void lookLeftCenter();
void lookRightCenter();
void realignCenter();
void captureImgLeft();
void captureImgRight();
void captureImgCenter();
void captureImgLeftCenter();
void captureImgRightCenter();
void initiateCrossing();
void crossTrack();
void avoidRobot();
void approachTrack();

#endif
