#include <pob-eye.h>

#include "joystick.h"

// Initialize the proto to have some properties which allow use and navigation by joystick
void SetupJoystick() {
  PobProto proto;
  
  // Set PORTA as analog so we get value of joystick back
  proto.porta = ALL_PORTA_AS_ANA;
  // Set all PORTC to use servos
  proto.portc = RC7_AS_DO | RC6_AS_DO | RC3_AS_DO | RC2_AS_SERVO | RC1_AS_SERVO | RC0_AS_SERVO;
  //RD0 RD1 RD2 RD3 are configured as digitals output to gear DC motor, RD4 RD5 RD6 RD7 are configured as digitals input
  proto.portd=RD7_AS_DI | RD6_AS_DI | RD5_AS_DI | RD4_AS_DI | RD3_AS_DO | RD2_AS_DO | RD1_AS_DO | RD0_AS_DO;
  // Set the above properties as the current proto
  SetPobProto(&proto);
}

// Pass off movement to PORTD
void MoveBot(UInt8 Direction) {
  SetPortD(Direction);
}

