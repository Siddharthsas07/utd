#include "dodrio.h"

#include <pob-eye.h>
#include "joystick.h"
#include "pattern.h"
#include "bitmap.h"

//Constant for movement loops
#define LOOP_ROTATE 330000
#define LOOP_FORWARD 330000

/*Struct to hold all images*/
int canCross[]={0,0,0,0,0};

UInt8 numFormsThisFrame = 0;
UInt8 formsThisFrameIdx = 0;
Form  formsListThisFrame[MAX_OF_FORM];

// Struct of three pointers on the RGB components
UInt8 * cameraFrame;
 
//This buffers will stock the pixels to display
UInt8 LCD_Buffer[LCD_WIDTH*LCD_HEIGHT*BITS];
GraphicBuffer screenBuffer;

int getCanCross(int i) {
  return canCross[i];
}

void initCameraAndLCD();

void initRobit() {
  //Initialize POB-EYE (lib), POB-LCD (lib), POB-PROTO(file Functions.c) and Servomotors
  InitPobeye2();
  InitI2C(I2C_100_KHZ);
  InitCameraPobeye2();
  InitLCD();      
  // Even though this project doesn't directly use the joystick, still set up
  // the ports to allow reading/writing of values to the DC motors
  SetupJoystick();
  SwitchOnAllServo();
  initCameraAndLCD();  
}

void rotateLeft(int deg) {
  MoveBot(LEFT); 
  //WaitUs(deg/10*LOOP_ROTATE);
  stopMovement();
}

void rotateRight(int deg) {
  MoveBot(RIGHT);
  //WaitUs(deg/10*LOOP_ROTATE);
  stopMovement();
}

void moveForward(int dist) {
  MoveBot(FORWARD);
  //WaitUs(dist*LOOP_FORWARD);
  stopMovement();
}

void moveBackward() {
  MoveBot(BACK);
}

// Each of the above rotate/move functions stays ON until stopMovement() is called
void stopMovement() {
  MoveBot(STOP);
}

void initCameraAndLCD() {
  // Get the pointer of the red,green and blue video buffer
  cameraFrame = GetRGBFrame();
  
  // Initialize the Graphic buffer with 128 per 64, one pixel per bit and LCD_Buffer
  // And clear the buffer, draw it, to make the screen clean
  InitGraphicBuffer(&screenBuffer, LCD_WIDTH, LCD_HEIGHT, ONE_BIT, LCD_Buffer);
  ClearGraphicBuffer(&screenBuffer);
}

void captureImage() {
  // grab the RGB components from the camera
  GrabRGBFrame();        
}

void decipherImage(int direction) {
  // Convert RGB camera frame to binary
  BinaryRGBFrame(cameraFrame); 
  //WaitUs(3000000);
  // Try to identify the forms and make a list of it.
  // CAN IDENTIFY MULTIPLE FORMS IN THE SAME IMAGE.
  // pattern defined in pattern.h
  numFormsThisFrame = IdentifyForm(cameraFrame, formsListThisFrame, pattern);  

  // No forms identified this frame
  if (numFormsThisFrame == 0) {
    DrawBitmap(0, 0, IDB_NOFORMS, bitmap, &screenBuffer);
    DrawLCD(&screenBuffer);
    canCross[direction]=-1;
  }
  
  // Parse the list of the form and print result on the Pob-Terminal and the LCD Screen
  // Draw bitmap and store into screenBuffer, then output screenBuffer
  // to the LCD screen. Note: This displays BIGA when a Trefle is observed?
  for (formsThisFrameIdx = 0; formsThisFrameIdx < numFormsThisFrame; formsThisFrameIdx++) {
  // Switch on the Pattern ID observed this frame
    switch (formsListThisFrame[formsThisFrameIdx].id) {
	 // TRACK
      case IDP_0_CROSS:
        DrawBitmap(0, 0, IDB_CROSS, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        canCross[direction]=IDP_0_CROSS;
        break;  
        
      // ROBOT        
      case IDP_1_BIGA:
        DrawBitmap(0, 0, IDB_BIGA, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        canCross[direction]=IDP_5_TRIANGLE;
        break;
        
      // TRAIN
      case IDP_2_KING:
        DrawBitmap(0, 0, IDB_KING, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        canCross[direction]=IDP_2_KING;
        break;
      
      // TRACK
      case IDP_3_TOWER:
        DrawBitmap(0, 0, IDB_TOWER, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        canCross[direction]=IDP_0_CROSS;
        break; 
      
      // NOT USED
      case IDP_4_TREFLE:
        DrawBitmap(0, 0, IDB_BIGA, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        break; 
      
      // ROBOT  
      case IDP_5_TRIANGLE:
        DrawBitmap(0, 0, IDB_TRIANGLE, bitmap, &screenBuffer);
        DrawLCD(&screenBuffer);
        canCross[direction]=IDP_5_TRIANGLE;
        break;
      
      // SAFE
      default:
        canCross[direction]=-1;
        break;
    }        
  }
}
