#ifndef JOYSTICK_H
#define JOYSTICK_H

//the right-left axe is on the RA1 analogic input
#define RIGHT_LEFT_AXIS 1

//the up-down axe is on the RA0 analogic input
#define UP_DOWN_AXIS 0

//the button is on the RA4 analogic input
#define BUTTON	4						

// Directions for the robot to move
#define FORWARD 0x05
#define STOP 0
#define LEFT 0x09
#define RIGHT 0x06
#define BACK 0x0A

#include <pob-eye.h>

void SetupJoystick();
void MoveBot(UInt8 Direction);

#endif
