#ifndef _ARM_H_
#define _ARM_H_

#include "main.h"

void shoulderMove(int shoulder) {
  motorSet(5, -shoulder);
}

void elbowMove(int elbow) {
  motorSet(4, -elbow);
}

void wristSet(int wrist) {
  motorSet(3, wrist);
}

void clawSet(int claw) {
  motorSet(2, claw);
}


#endif // _CHASSIS_H_
