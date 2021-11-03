/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"
#include "chassis.h"
#include "math.h"
#include "arm.h"

void operatorControl() {
  Encoder shoulderEncoder;
  shoulderEncoder = encoderInit(SHOULDER_ENCODER_TOP_PORT, SHOULDER_ENCODER_LOWER_PORT, false);
  Encoder elbowEncoder;
  elbowEncoder = encoderInit(ELBOW_ENCODER_TOP_PORT, ELBOW_ENCODER_LOWER_PORT, false);
	int power;
	int turn;
	int shoulder;
	int elbow;
	int wrist;
	int claw;
  int elbowCounts;
  int shoulderCounts;
  int LShoul;
  int LElb;
  int targetX = 10;
  int targetY = 1;
  int targetS;
  int targetE;
  int errorS;
  int errorE;
  int straight;
  double D;
  LShoul = 21;
  LElb = 28;
	while (1) {
    //CONTROLS
		power = joystickGetAnalog(1, 1);
		turn = -joystickGetAnalog(1, 2);
		shoulder = joystickGetAnalog(1, 3);
		elbow = joystickGetAnalog(1, 4);
		wrist = 100 * (joystickGetDigital(1, 5, JOY_UP) - joystickGetDigital(1, 5, JOY_DOWN));
		claw = 100 * (joystickGetDigital(1, 6, JOY_UP) - joystickGetDigital(1, 6, JOY_DOWN));
		chassisSet(power, turn);
		shoulderMove(shoulder);
		wristSet(wrist);
		clawSet(claw);
		elbowMove(elbow);
    //END CONTROLS
    //HOMING
		if (joystickGetDigital(1, 8, JOY_UP) == 1){
      shoulderMove(50);
      elbowMove(-50);
      delay(1000);
      elbowMove(0);
      while(digitalRead(SHOULDER_LIMIT_SWITCH) == HIGH){
        shoulderMove(50);
        printf("shoulder Enc %d\n", encoderGet(shoulderEncoder));
        delay(100);
      }
      printf("UpperLimitSwitchisPressed \n");
      encoderReset(shoulderEncoder);
      while((encoderGet(shoulderEncoder))<132){
        printf("shoulder encoder %d\n", encoderGet(shoulderEncoder));
        shoulderMove(-165+(encoderGet(shoulderEncoder)));
        delay(150);
      }
      encoderReset(shoulderEncoder);
      shoulderMove(encoderGet(shoulderEncoder)-0);
      while(digitalRead(ELBOW_LIMIT_SWITCH) == HIGH){
        elbowMove(-50);
        printf("elbow encoder %d\n", encoderGet(elbowEncoder));
        shoulderMove(encoderGet(shoulderEncoder)-0);
        delay(150);
      }
      encoderReset(elbowEncoder);
      while((encoderGet(elbowEncoder))<205){
        elbowMove(205-(encoderGet(elbowEncoder)));
      }
      encoderReset(elbowEncoder);
      while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
        shoulderMove(-2*(0-encoderGet(shoulderEncoder)));
        elbowMove(2*(0-encoderGet(elbowEncoder)));
        printf("elbow move %d\n", 0-encoderGet(elbowEncoder));
        printf("shoulder move %d\n", 0-encoderGet(shoulderEncoder));
      }
		}
    //END HOMING
    //PRINT
    if(joystickGetDigital(1, 8, JOY_DOWN) == 1){
      printf("shoulder enc %d\n", encoderGet(shoulderEncoder));
      printf("elbow enc %d\n", encoderGet(elbowEncoder));
    }
    //CLC
    if (joystickGetDigital(1, 8, JOY_LEFT) == 1) {
      elbowCounts = encoderGet(elbowEncoder);
      shoulderCounts = encoderGet(shoulderEncoder);
      while(joystickGetDigital(1, 8, JOY_RIGHT) == 0){
        shoulderMove(-4*(shoulderCounts-encoderGet(shoulderEncoder)));
        elbowMove(4*(elbowCounts-encoderGet(elbowEncoder)));
      }
    }
    //END CLC
    //STRAIGHT line
    if (joystickGetDigital(1, 7, JOY_UP) == 1){
      printf("elbowwww %d\n", encoderGet(elbowEncoder));
      printf("shoulderrrr %d\n", encoderGet(shoulderEncoder));
    }
    if (joystickGetDigital(1, 7, JOY_LEFT) == 1){
      while(joystickGetDigital(1, 7, JOY_DOWN) == 0){
        D = ((targetX*targetX) + (targetY*targetY) - (LShoul*LShoul) - (LElb*LElb))/(targetY*LShoul*LElb);
        wait(10);
        targetE = (180/3.1415)*atan2((sqrt(1-(D*D))),D);
        wait(10);
        targetS = -(180/3.1415)*(atan2(targetY, targetX) + atan2((LElb*sin(targetE)), (LShoul+LElb*cos(targetE))));
        wait(10);
        errorE = targetE - encoderGet(elbowEncoder);
        errorS = targetS - encoderGet(shoulderEncoder);
        if(joystickGetDigital(1, 7, JOY_LEFT) == 1){
          printf("Target E %d\n", targetE);
          printf("error E %d\n", errorE);
          printf("Target S %d\n", targetS);
          printf("error S %d\n", errorS);
        }
        if(joystickGetDigital(1, 7, JOY_RIGHT) == 1){
          elbowMove(4*errorE);
          shoulderMove(-4*errorS);
          wait(100);
          shoulderMove(0);
          elbowMove(0);
          D = ((targetX*targetX) + (targetY*targetY) - (LShoul*LShoul) - (LElb*LElb))/(2*LShoul*LElb);
          wait(10);
          targetE = 2*(180/3.1415)*atan2((sqrt(1-(D*D))),D);
          wait(10);
          targetS = -(180/3.1415)*(atan2(targetY, targetX) - atan2((LElb*sin(targetE)), (LShoul+LElb*cos(targetE))));
          wait(10);
          errorE = targetE - encoderGet(elbowEncoder);
          errorS = targetS - encoderGet(shoulderEncoder);
        }
        if(joystickGetDigital(1, 5, JOY_UP) == 1){
          targetX = 5;
          targetY = 0;
          straight = 0;
          wait(10);
          while(straight < 30){
            targetX = 5 - straight;
            targetY = 0 + straight;
            D = ((targetX*targetX) + (targetY*targetY) - (LShoul*LShoul) - (LElb*LElb))/(2*LShoul*LElb);
            wait(5);
            targetE = 2*(180/3.1415)*atan2((sqrt(1-(D*D))),D);
            wait(5);
            targetS = -(180/3.1415)*(atan2(targetY, targetX) - atan2((LElb*sin(targetE)), (LShoul+LElb*cos(targetE))));
            wait(5);
            errorE = targetE - encoderGet(elbowEncoder);
            errorS = targetS - encoderGet(shoulderEncoder);
            wait(10);
            elbowMove(4*errorE);
            shoulderMove(-4*errorS);
            wait(100);
            straight += .5;
          }
        }
        if(joystickGetDigital(1, 8, JOY_UP) == 1){
          targetX = -10;
          targetY = -10;
          D = ((targetX*targetX) + (targetY*targetY) - (LShoul*LShoul) - (LElb*LElb))/(2*LShoul*LElb);
          wait(10);
          targetE = 2*(180/3.1415)*atan2((sqrt(1-(D*D))),D);
          wait(10);
          targetS = -(180/3.1415)*(atan2(targetY, targetX) - atan2((LElb*sin(targetE)), (LShoul+LElb*cos(targetE))));
          wait(10);
          errorE = targetE - encoderGet(elbowEncoder);
          errorS = targetS - encoderGet(shoulderEncoder);
        }
  //  shoulderMove(shoulder);
  //  elbowMove((encoderGet(shoulderEncoder)+asin(LShoul*(sin(encoderGet(elbowEncoder)*3.1415/180))/LElb))-encoderGet(elbowEncoder));
      }
    }
    //END STRAIGHT LINE
	}
}

/*
int l1
int l2

int targetX = 7;
int targetY = 0;
int targetS :
int targetE
int errorS ;
int errorE ;
int kPE;
int kPS;
int outputE
int outputS
double D;

//use targetX and targetY to calculate targetE and targetS
D = ((targetX)^2 + (targetY)^2 - (l1)^2 - (l2)^2)/2*l1*l2
targetE = (180/3.1415)*atan2(sqtr(1-D)D )
targetS = (180/3.1415)*(atan2(targetY/targetX) - atan2((L2*sin(targetE))/(L1+cos(targetE))))
errorE = TargetE - elbowEncoder
errorS = TargetS - shoulderEncoder

 printf("the targets are x y E and S %d %d %d %d \n" targetX, targetY, targetE, targetS)
// use targetE and targetS to calculate errorE and errorS using encoder date
// multiply kPE*errorE and kPS*errorS to find motor outputs
//set motor power = to outputs


*/


// delay(900);
// }
