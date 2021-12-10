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
  elbowEncoder = encoderInit(ELBOW_ENCODER_TOP_PORT, ELBOW_ENCODER_LOWER_PORT, true);
  Ultrasonic Sonar;
  Sonar = ultrasonicInit(1, 2);
  int LLine = 8;
  int MLine = 9;
  int RLine = 10;
  analogCalibrate(LLine);
  analogCalibrate(MLine);
  analogCalibrate(RLine);
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
  double targetX;
  double targetY;
  int targetS;
  int targetE;
  int theta1;
  int theta2;
  int errorS;
  int errorE;
  int distance;
  int lineturn;
  double D;
  targetX = 20;
  targetY = 31;
  LShoul = 26;
  LElb = 33;
	while(1) {
    //CONTROLS
		power = joystickGetAnalog(1, 1);
		turn = -joystickGetAnalog(1, 2);
		shoulder = -joystickGetAnalog(1, 3);
		elbow = joystickGetAnalog(1, 4);
		wrist = 100 * (joystickGetDigital(1, 5, JOY_UP) - joystickGetDigital(1, 5, JOY_DOWN));
		claw = 100 * (joystickGetDigital(1, 6, JOY_UP) - joystickGetDigital(1, 6, JOY_DOWN));
		chassisSet(power, turn);
		shoulderMove(shoulder);
		wristSet(wrist);
		clawSet(claw);
		elbowMove(-elbow);

    //HOMING
		if(joystickGetDigital(1, 8, JOY_UP) == 1){
      shoulderMove(-50);
      elbowMove(50);
      delay(1000);
      elbowMove(0);
      while(digitalRead(SHOULDER_LIMIT_SWITCH) == HIGH){
        shoulderMove(-50);
        elbowMove(0);
        printf("shoulder Enc %d\n", encoderGet(shoulderEncoder));
        delay(100);
      }
      printf("UpperLimitSwitchisPressed \n");
      encoderReset(shoulderEncoder);
      while((encoderGet(shoulderEncoder))<132){
        printf("shoulder encoder %d\n", encoderGet(shoulderEncoder));
        shoulderMove(165-(encoderGet(shoulderEncoder)));
        delay(150);
      }
      encoderReset(shoulderEncoder);
      shoulderMove(0-encoderGet(shoulderEncoder));
      while(digitalRead(ELBOW_LIMIT_SWITCH) == HIGH){
        elbowMove(50);
        printf("elbow encoder %d\n", encoderGet(elbowEncoder));
        shoulderMove(0-encoderGet(shoulderEncoder));
        delay(150);
      }
      encoderReset(elbowEncoder);
      while((encoderGet(elbowEncoder))>-205){
        elbowMove(-205-(encoderGet(elbowEncoder)));
      }
      encoderReset(elbowEncoder);
      while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
        shoulderMove(2*(0-encoderGet(shoulderEncoder)));
        elbowMove(2*(0-encoderGet(elbowEncoder)));
        printf("elbow move %d\n", 0-encoderGet(elbowEncoder));
        printf("shoulder move %d\n", 0-encoderGet(shoulderEncoder));
      }
		}
    //CLC
    if(joystickGetDigital(1, 8, JOY_LEFT) == 1) {
      elbowCounts = encoderGet(elbowEncoder);
      shoulderCounts = encoderGet(shoulderEncoder);
      while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
        shoulderMove(4*(shoulderCounts-encoderGet(shoulderEncoder)));
        elbowMove(-4*(elbowCounts-encoderGet(elbowEncoder)));
      }
    }
    //STRAIGHT LINE TRIAL 2
    if(joystickGetDigital(1, 8, JOY_RIGHT) == 1){
      targetX = 0;
      targetY = 31;
      while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
        D = ((targetX*targetX) + (targetY*targetY) - (LShoul*LShoul) - (LElb*LElb))/(2*LShoul*LElb);
        theta2 = (180/3.1415)*(acos(D));
        theta1 = (180/3.1415)*(atan2(targetY, targetX) + atan2(LElb*cos(3.1415*theta2/180), (LShoul + (LElb*cos(3.1415*theta2/180)))));
        targetS = round(theta1);
        targetE = round(theta1 - theta2);
        errorE = round(targetE - encoderGet(elbowEncoder));
        errorS = round(targetS - encoderGet(shoulderEncoder));
        shoulderMove(4 * errorS);
        elbowMove(4 * errorE);
        while(joystickGetDigital(1, 8, JOY_LEFT) == 1){
          printf("theta2 %d\n", theta2);
          wait(5);
          printf("theta1 %d\n", theta1);
          wait(5);
          printf("targetS %d\n", targetS);
          wait(5);
          printf("targetE %d\n", targetE);
          wait(5);
          printf("ShoulderEnc %d\n", encoderGet(shoulderEncoder));
          wait(5);
          printf("ElbowEnc %d\n", encoderGet(elbowEncoder));
          wait(5);
          printf("errorE %d\n", errorE);
          wait(5);
          printf("errorS %d\n", errorS);
          wait(5);
        }
        if(joystickGetDigital(1, 7, JOY_LEFT) == 1){
          targetX = targetX + 2;
          wait(10);
        }
      }
    }
    //FOLLOW USR SENSOR
    if(joystickGetDigital(1, 7, JOY_LEFT) == 1){
      while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
        while(encoderGet(shoulderEncoder) < 125 ^ encoderGet(elbowEncoder) < 0){
          shoulderMove(125 - encoderGet(shoulderEncoder));
          elbowMove(0 - encoderGet(elbowEncoder));
        }
        distance = ultrasonicGet(Sonar);
        while(distance < 100){
          chassisSet(-50 + distance, 0);
        }
        while(distance > 100){
          chassisSet(25, -1);
          wait(6000);
          chassisSet(30, 1);
          wait(6000);
        }
      }
    }
    //FOLLOW LINE SENSOR
    while(joystickGetDigital(1, 7, JOY_RIGHT) == 1){
      shoulderMove(50);
      elbowMove(50);
    }
	}
}
