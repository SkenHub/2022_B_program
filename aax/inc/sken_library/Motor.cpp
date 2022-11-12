/*
 * Motor.cpp
 *
 *  Created on: 2021/06/23
 *      Author: hotaka
 */

/*
 * モーターを制御するクラス
*/

#include "Motor.h"

Motor::Motor(){}

void Motor::init(Pin pin1,Pin pinP,Pin pin2,TimerNumber timer,TimerChannel ch){
	motor[0].init(pin1,OUTPUT);
	motor[1].init(pinP,PWM_OUTPUT,timer,ch);
	motor[2].init(pin2,OUTPUT);
}

void Motor::write(int val){
	if(val == 0){
		motor[0].write(LOW);
		motor[1].write(val);
		motor[2].write(LOW);
	}else if(val > 0){
		motor[0].write(HIGH);
		motor[1].write(val);
		motor[2].write(LOW);
	}else if(val < 0){
		motor[0].write(LOW);
		motor[1].write(-1*val);
		motor[2].write(HIGH);
	}
}

void Motor::stop(){
	motor[0].write(HIGH);
	motor[1].write(0);
	motor[2].write(HIGH);
}
