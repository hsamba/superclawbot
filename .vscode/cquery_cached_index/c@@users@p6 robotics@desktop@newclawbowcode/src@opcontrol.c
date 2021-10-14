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
#include "arm.h"

void homeshoulder(){
  Encoder shoulderEncoder;
	shoulderEncoder = encoderInit(SHOULDER_ENCODER_TOP_PORT, SHOULDER_ENCODER_LOWER_PORT, false);
	Encoder elbowEncoder;
	elbowEncoder = encoderInit(ELBOW_ENCODER_TOP_PORT, ELBOW_ENCODER_LOWER_PORT, false);
  int elbowCounts;
  int shoulderCounts;
  while (joystickGetDigital(1, 7, JOY_UP) == 0 & joystickGetDigital(1, 7, JOY_DOWN) == 0){
    wait(1);
  }
  if(joystickGetDigital(1, 7, JOY_UP) == 1){
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
	while((encoderGet(shoulderEncoder))<125){
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
  while((encoderGet(elbowEncoder))<75){
		elbowMove(100-(encoderGet(elbowEncoder)));
	 }
	 encoderReset(elbowEncoder);
    while(joystickGetDigital(1, 8, JOY_DOWN) == 0){
      shoulderMove(2*(0-encoderGet(shoulderEncoder)));
      elbowMove(2*(0-encoderGet(elbowEncoder)));
    }
  } else if (joystickGetDigital(1, 7, JOY_DOWN) == 1) {
  elbowCounts = encoderGet(elbowEncoder);
  shoulderCounts = encoderGet(shoulderEncoder);
  while(joystickGetDigital(1, 8, JOY_RIGHT) == 0){
    shoulderMove(-4*(shoulderCounts-encoderGet(shoulderEncoder)));
    elbowMove(4*(elbowCounts-encoderGet(elbowEncoder)));
    printf("elbow %d\n", elbowCounts-encoderGet(elbowEncoder));
    printf("shoulder %d\n", shoulderCounts-encoderGet(shoulderEncoder));
    printf("shoulder enc %d\n", encoderGet(shoulderEncoder));
    printf("elbow enc %d\n", encoderGet(elbowEncoder));
    wait(1);
  }
}
}

void operatorControl() {
	int power;
	int turn;
	int shoulder;
	int elbow;
	int wrist;
	int claw;
	while (1) {
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
		if (joystickGetDigital(1, 8, JOY_UP) == 1){
				homeshoulder();
		}
	}
}

//* if (digitalRead(SHOULDER_LIMIT_SWITCH) == LOW){
// printf("UpperLimitSwitchisPressed \n");
// printf("%d\n", encoderGet(shoulderEncoder));
// encoderReset(shoulderEncoder);
// delay(900);
// }
