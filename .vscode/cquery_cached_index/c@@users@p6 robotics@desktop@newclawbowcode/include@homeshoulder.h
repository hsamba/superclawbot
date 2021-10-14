#include "main.h"

void shoulderHome(){
  printf("homing started, standby \n");
  while(analogRead(LIMIT_SWITCH)==HIGH){
    shoulderMove(50);
  }
  encoderReset(shoulderEncoder);

}
