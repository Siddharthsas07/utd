#ifndef DODRIO_H
#define DODRIO_H

//Check wether it is safe to cross in a given direction
int getCanCross(int dir);

// Initialize the servos, camera, and LCD
void initRobit();

// Rotate the robit left or right
void rotateLeft();
void rotateRight();

// Move the robit foward or backward
void moveForward();
void moveBackward();

// Stops rotation and movement. Rotation and movement functions persist
// until this function is called.
void stopMovement();

// Capture an image.
void captureImage();

// Decipher image and output identified shape(s) onto the LCD.
void decipherImage(int direction);

#endif