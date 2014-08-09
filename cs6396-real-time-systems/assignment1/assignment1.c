/* CE 6308/CS 6396/EE 6308: REAL TIME SYSTEMS 
   Assignment 1 - Control of a Train using Tornado developmental environment and WindRiver RTOS

   Members of Group 3:   Mark Baker, 
                         Gary Steelman,
                         Melyssa Fowler, 
                         Jie Fang
*/

#include <taskLib.h>
#include <logLib.h>
#include <ioLib.h>
#include <msgQLib.h>
#include <stdio.h>

/* Variable & Function Declarations*/
unsigned char SerBuf[3],x;
int ser_fd;

typedef enum {START=1,BELL,STOP,ACCELERATE,DECELERATE,MOVE} command;
void PrintMenu();
void GetSelection();
void SendCommand();

/*Global Data Structures*/
#define ENGINE_ADDRESS 1

/* Inititalization - opens a port and if successful displays menu for further operations */
void init(void)
{
   /* Open Serial Port */
    ser_fd =  open("/tyCo/1",O_WRONLY, 0);
  printf("\nTrain id = %d",ENGINE_ADDRESS);
    if( ser_fd == -1)
        printf("\nSerial Port Not Open, error\n",ser_fd,0,0,0,0,0);
    else
        printf("\nSerial Port Open, fd = %d \n",ser_fd,0,0,0,0,0);
  
  /* Main functions */
  while(1)
  {
    command selected= START;
    PrintMenu();
    GetSelection(&selected);
    /* Train Commands - The bit pattern specified in the TrainMaster manual */
    switch(selected)
    {
      case START:
      case MOVE:
        SerBuf[2] = 0xe8;
        break;
      case BELL:
        SerBuf[2] = 0x9d;
        break;
      case ACCELERATE:
        SerBuf[2] = 0xc7;
        break;
      case DECELERATE:
        SerBuf[2] = 0xc3;
        break;
      case STOP:
        SerBuf[2] = 0xe0;
        break;
      default:
        printf("WARNING - You fail at typing in a number 1 - 6.\n\n"); 
        break;
    }
    SendCommand();
  }
}

/* Function used to generate the Train Command Menu */
void PrintMenu()
{
  printf("\n-----------------------------------------------------------------------\n");
  printf("       Commands for Controlling the Train                             \n");
  printf("-----------------------------------------------------------------------\n");
  printf("\t1.\tStart\n");
  printf("\t2.\tToggle Bell\n");
  printf("\t3.\tStop\n");
  printf("\t4.\tAccelerate\n");
  printf("\t5.\tDecelerate\n");
  printf("\t6.\tMove\n");
  printf("-----------------------------------------------------------------------\n");
     printf("Please choose a command: ");
}

/* Function used to receive selected commands */
void GetSelection(command* choice)
{
  scanf("%d",choice);  
}

/* Function used to send commands to the train */
void SendCommand()
{
  int numByteWritten = 0;
    SerBuf[0] = (unsigned char)254;
    SerBuf[1] = (unsigned char)0;
  numByteWritten = write(ser_fd, SerBuf, 3);
  
} 


/*      END OF PROGRAM              */
