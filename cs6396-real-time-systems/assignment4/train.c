/* ============================== Train.c ============================== */
#include "vxWorks.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"
#include "tcpExample.h"
#include "msgQLib.h"
#include <taskLib.h>
#include <logLib.h>
#include <ioLib.h>
#include <msgQLib.h>
#include <stdio.h>
#include <time.h>

/* Function Declarations */
/* Global Data Structures */
#define ENGINE_ADDRESS 1
int counter=0;
time_t t1, t2;

/* Variable & Function Declarations*/
unsigned char SerBuf[3],x;
int ser_fd; 
void turnOnLights();
void turnOffLights();
void accelerateOneStage();
void decelerateOneStage();
void blowHornOnce();
void wait60sec();
void sendCommand();

/* function declarations */
void tcpServerWorkTask (int sFd, char * address, u_short port);

/****************************************************************************
*
* tcpServer - accept and process requests over a TCP socket
*
* This routine creates a TCP socket, and accepts connections over the socket
* from clients. Each client connection is handled by spawning a separate
* task to handle client requests.
*
* This routine may be invoked as follows:
* -> sp tcpServer
* task spawned: id = 0x3a6f1c, name = t1
* value = 3829532 = 0x3a6f1c
* -> MESSAGE FROM CLIENT (Internet Address 150.12.0.10, port 1027):
* Hello out there
*
* RETURNS: Never, or ERROR if a resources could not be allocated.
*/STATUS tcpServer (void) {
  struct sockaddr_in serverAddr; /* server's socket address */
  struct sockaddr_in clientAddr; /* client's socket address */
  int sockAddrSize; /* size of socket address structure */
  int sFd; /* socket file descriptor */
  int newFd; /* socket descriptor from accept */
  int ix = 0; /* counter for work task names */
  char workName[16]; /* name of work task */
  int STOP_DISTANCE = 40; /* distance from the sensor to start slowing down */
  int NUM_SPEED_STAGES = 1; /* number of speed stages to accel/decelerate */
  int i,j,l; 
  char sensorData;
  printf("TCP PCPCPCPCP PCAT CPTP");
  /* set up the local address */
  sockAddrSize = sizeof (struct sockaddr_in);
  bzero ((char *) &serverAddr, sockAddrSize);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_len = (u_char) sockAddrSize;
  serverAddr.sin_port = htons (SERVER_PORT_NUM);
  serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  
  /* create a TCP-based socket */
  if ((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR) {
    perror ("socket");
    return (ERROR);
  }
  
  /* bind socket to local address */
  if (bind (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
    perror ("bind");
    close (sFd);
    return (ERROR);
  }
  
  /* create queue for client connection requests */
  if (listen (sFd, SERVER_MAX_CONNECTIONS) == ERROR) {
    perror ("listen");
    close (sFd);
    return (ERROR);
  }
  
  /* accept new connect requests and spawn tasks to process them */ 
  if ((newFd = accept (sFd, (struct sockaddr *) &clientAddr, &sockAddrSize)) == ERROR) {
    perror ("accept");
    close (sFd);
    return (ERROR);
  }
  
  sprintf (workName, "tTcpWork%d", ix++);
  printf("set up connection with tornado server\n");
  
  /* Open Serial Port */
  ser_fd = open("/tyCo/1",O_WRONLY, 0);printf("\nTrain id = %d",ENGINE_ADDRESS);
  if (ser_fd == -1)
    printf("\nSerial Port Not Open, error\n",ser_fd,0,0,0,0,0);
  else
    printf("\nSerial Port Open, fd = %d \n",ser_fd,0,0,0,0,0);

  /* Get the train moving to crusising speed */
  for (i = 0; i < NUM_SPEED_STAGES; i++) {
    accelerateOneStage();
  }
  
  while(1) {
    fflush(newFd); 
    read(newFd, &sensorData, sizeof(int));
    
    /* If the train is close to the station (arrival process) */
    if ((int)sensorData < STOP_DISTANCE) {
      turnOnLights();
      
      /* Decelerate to stop */
      for (i = 0; i < NUM_SPEED_STAGES; i++ ) {
        decelerateOneStage();
      }
      
      blowHornOnce();
      
      /* The departure process starts around 60 seconds */
      wait60sec();
      blowHornOnce();
      
      /* Get the train moving to cruising speed */
      for (i = 0; i < NUM_SPEED_STAGES; i++ ) { 
        accelerateOneStage();
      }
      
      turnOffLights();
    }
  }
}

/* Activate Lights Task */
void turnOnLights() {
  SerBuf[2] = 0x8b;
  sendCommand();
}

/* Deactivate Lights Task */
void turnOffLights() {
  SerBuf[2] = 0x88;
  sendCommand();
}

/* Acceleration Task */
void accelerateOneStage() {
  SerBuf[2] = 0xc7;
  sendCommand();
}

/* Deceleration */
void decelerateOneStage() {
  SerBuf[2] = 0xc3;
  sendCommand();
}

/* Horn Task */
void blowHornOnce() {
  SerBuf[2] = 0x9c;
  sendCommand();
}

/* Wait Task */
void wait60sec() {
  sleep(60);
}

void sendCommand() {
  int numByteWritten = 0;
  SerBuf[0] = (unsigned char)254;
  SerBuf[1] = (unsigned char)0;
  numByteWritten = write(ser_fd, SerBuf, 3);
}