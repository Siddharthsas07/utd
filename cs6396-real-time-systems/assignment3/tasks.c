#include <stdio.h>
#include <pob-eye.h>
#include "tasks.h"
#include "cqueue.h"
#include "dodrio.h"
#include "pattern.h"
Task lookLeftTask = { .taskFunction = lookLeft, .executionTime = 4 };
Task lookRightTask = { .taskFunction = lookRight, .executionTime = 2 };
Task lookCenterTask = { .taskFunction = lookCenter, .executionTime = 2 };
Task lookLeftCenterTask = { .taskFunction = lookLeftCenter, .executionTime = 2 };
Task lookRightCenterTask = { .taskFunction = lookRightCenter, .executionTime = 2 };
Task realignCenterTask = { .taskFunction = realignCenter, .executionTime = 4 };
Task captureImgLeftTask = { .taskFunction = captureImgLeft, .executionTime = 1 };
Task captureImgRightTask = { .taskFunction = captureImgRight, .executionTime = 1 };
Task captureImgCenterTask = { .taskFunction = captureImgCenter, .executionTime = 1 };
Task captureImgLeftCenterTask = { .taskFunction = captureImgLeftCenter, .executionTime = 1 };
Task captureImgRightCenterTask = { .taskFunction = captureImgRightCenter, .executionTime = 1 };
Task approachTrackTask = { .taskFunction = approachTrack, .executionTime = 5 };
Task initiateCrossingTask = { .taskFunction = initiateCrossing, .executionTime = 1 };
Task crossTrackTask = { .taskFunction = crossTrack, .executionTime = 3 };
Task avoidRobotTask = { .taskFunction = avoidRobot, .executionTime = 10 };
/* LOOK TASKS */ 
void lookLeft()
{
	PrintTextOnPobTerminal("Look Left Task\n");
	rotateLeft(20);
	rotateLeft(20);
}
void lookRight()
{
	PrintTextOnPobTerminal("Look Right Task\n");
	rotateRight(20);
}
void lookCenter()
{
	PrintTextOnPobTerminal("Look Center Task\n");
	rotateRight(20);
}
void lookLeftCenter()
{
	PrintTextOnPobTerminal("Look Left Center Task\n");
	rotateRight(20);
}
void lookRightCenter()
{
	PrintTextOnPobTerminal("Look Right Center Task\n");
	rotateRight(20);
}
/* REALIGN TASKS */
void realignCenter()
{
	PrintTextOnPobTerminal("Realign Center Task\n");
	rotateLeft(20);
	rotateLeft(20);
}
/* CAPTURE IMG TASKS */
void captureImgLeft()
{
	PrintTextOnPobTerminal("Capture Image Left Task\n");
	captureImage();
	decipherImage(0);
}
void captureImgRight()
{
	PrintTextOnPobTerminal("Capture Image Right Task\n");
	captureImage();
	decipherImage(4);
}
void captureImgCenter()
{
	PrintTextOnPobTerminal("Capture Image Center Task\n");
	captureImage();
	decipherImage(2);
}
void captureImgLeftCenter()
{
	PrintTextOnPobTerminal("Capture Image Left Center Task\n");
	captureImage();
	decipherImage(1);
}
void captureImgRightCenter()
{
	PrintTextOnPobTerminal("Capture Image Right Center Task\n");
	captureImage();
	decipherImage(3);
}
/* DECIPHER IMG TASKS */ 
/* initiate crossing */
void initiateCrossing()
{
	static int debug = 0;
	debug++;
	PrintTextOnPobTerminal("Initiate Crossing Task\n");
	int i=0;
	int action=0;
	int temp=0;
	for(i=0;i<5;i++)
	{
		temp=getCanCross(i);
		if(temp==IDP_6_CIRCLE||temp==IDP_2_KING)
		{
			action=1;
			break;
		}
		if(temp==IDP_5_TRIANGLE||temp==IDP_1_BIGA)
		{
			action=2;
			break;
		}
	}
	if(action==2)
	{
		queueInsert(&avoidRobotTask,aperiodicQ);
	}
	if(action==0)
	{
		queueInsert(&crossTrackTask,sporadicQ);
	}
}
/* SPORADIC TASKS */
void crossTrack()
{
	PrintTextOnPobTerminal("Cross Track Task\n");
	//WaitUs(5000000);
	moveForward(10);
}
/* APERIODIC TASKS */
void avoidRobot()
{
	PrintTextOnPobTerminal("Avoid Robot Task\n");
	//WaitUs(5000000);
	rotateRight(20);
	rotateRight(20);
	rotateRight(10);
	moveForward(5);
	rotateLeft(20);
	rotateLeft(20);
}
void approachTrack()
{
	PrintTextOnPobTerminal("Approach Track Task\n");
	int d=-1;	
	while(d!=IDP_0_CROSS)
	{	
		captureImage();
		decipherImage(2);
		d=getCanCross(2);
		moveForward(1);
	}
}
