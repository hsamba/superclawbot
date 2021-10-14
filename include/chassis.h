#ifndef _CHASSIS_H_
#define _CHASSIS_H_

#include "main.h"


void chassisSet(power, turn) {
  motorSet(8, power - turn);
  motorSet(9, power + turn);
}


#endif // _CHASSIS_H_
