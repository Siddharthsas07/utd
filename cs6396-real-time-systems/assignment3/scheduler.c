//
//  scheduler.c
//  
//
//  Created by Melyssa Son on 3/21/13.
//
//

#include <stdio.h>

#include "scheduler.h"
#include "cqueue.h"

Task * tasks[FRAMES][MAX_TASKS_PER_FRAME] =
{   /* Frame 1*/ {&lookLeftTask, &captureImgLeftTask, &lookLeftCenterTask, &captureImgLeftCenterTask, 0, 0, 0, 0, 0, 0},
    /* Frame 2*/ {&lookCenterTask, &captureImgCenterTask, &lookRightCenterTask, &captureImgRightCenterTask, &lookRightTask, 0, 0, 0, 0, 0},
    /* Frame 3*/ {&captureImgRightTask,&realignCenterTask,&initiateCrossingTask, 0, 0, 0, 0, 0, 0, 0},
    /* Frame 4*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void scheduler()
{
    int t = 0;          /* current time */
    int k = 0;          /* current frame */
    int taskIdx = 0;    /* keeps track what task we're on */
    int idle = 1;       /* keeps track if we executed a task or not */
    int continueLoop=1;
    while(continueLoop)
    {
        idle = 1;
        //sleep(1); // FOR DEBUG ONLY!
        
        /* t is not incremented here like it is in the book, this is
         because we don't have timers. t is incremented by the execution
         time when a task is scheduled or when it idles */
        k = (t/FRAME_SIZE)%FRAMES;
        
        /* Assumption: no tasks run over... since we don't have a timer, this
         has to be true */
        
        /* execute PERIODIC tasks */
        if(tasks[k][taskIdx] != 0 && taskIdx < MAX_TASKS_PER_FRAME)
        {
            /* simulate the passage of time */
            //printf("t=%d frame=%d \texecuting PERIODIC task: \t", t, k);
            t += tasks[k][taskIdx]->executionTime;
            tasks[k][taskIdx]->taskFunction();
            idle = 0;
            
            /* increment taskIdx */
            if(k == (t/FRAME_SIZE)%FRAMES)
            {
                taskIdx++;
            }
            else
            {
                taskIdx = 0; 
            }
            
        } 
        else   
        {
            /* execute APERIODIC tasks */
            if(!isEmpty(aperiodicQ)) 
            {
                int remainingTime = FRAME_SIZE - (t%FRAME_SIZE);
                
            
                /* Since we can't premept jobs in this setup, don't run aperiodic tasks
                 unless you have enough time in the frame */
            
                Task * newTask = queuePeek(aperiodicQ);
                if(newTask->executionTime <= remainingTime)
                {
                    /* execute it */
                    //printf("t=%d frame=%d \texecuting APERIODIC task: \t", t, k);
                    t += newTask->executionTime;
                    newTask->taskFunction();
                    queueRemove(aperiodicQ);
                    idle = 0;
                }
            }
            
            /* execute SPORADIC tasks */
            if(!isEmpty(sporadicQ)) 
            {
			 continueLoop=0;                
                int remainingTime = FRAME_SIZE - (t%FRAME_SIZE);
                
                /* This is the major difference between the sporadic and aperiodic
                 tasks: If we don't have time to run the sporadic task, it doesn't
                 get run at all because late is worse than never, and is thus 
                 removed whether it runs or not */ 
                
                Task * newTask = queueRemove(sporadicQ);
                
                if(newTask->executionTime <= remainingTime)
                {
                    /* execute it */
                   //printf("t=%d frame=%d \texecuting SPORADIC task: \t", t, k);
                    t += newTask->executionTime;
                    newTask->taskFunction();
                    idle = 0;

                }
                else
                {
                   //printf("t=%d frame=%d \t!!!!DISCARDED SPORADIC TASK!!! ", t, k);
                }
            }
                
        }
        
        if(idle)
        {
            /* no tasks were executed, advance the clock anyways */
             //printf("t=%d frame=%d \tIdling...\n", t, k);
            t++;
        }

        
        //reset taskIdx if needed
        if(k != (t/FRAME_SIZE)%FRAMES)
        {
            taskIdx = 0;
        }
        
    }
   
}
