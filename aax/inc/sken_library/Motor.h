/*
 * Motor.h
 *
 *  Created on: 2021/06/23
 *      Author: hotaka
 */

/*
 * モーターを制御するクラス
*/

#ifndef MOTOR_H_
#define MOTOR_H_

#include "sken_library/Gpio.h"

class Motor {
public:
	Motor();
	void init(Pin pin1,Pin pinP,Pin pin2,TimerNumber timer,TimerChannel ch);
	void write(int val);
	void stop();
private:
	Gpio motor[3];
};

#endif /* MOTOR_H_ */
