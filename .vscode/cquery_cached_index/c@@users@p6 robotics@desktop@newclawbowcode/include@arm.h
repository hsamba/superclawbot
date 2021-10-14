#ifndef _ARM_H_
#define _ARM_H_

#include "main.h"

void shoulderMove(shoulder) {
  motorSet(5, shoulder);
}

void elbowMove(elbow) {
  motorSet(4, elbow);
}

void wristSet(wrist) {
  motorSet(3, wrist);
}

void clawSet(claw) {
  motorSet(2, claw);
}


#endif // _CHASSIS_H_
