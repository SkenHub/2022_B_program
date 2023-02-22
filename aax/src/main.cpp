/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
/*
 プログラム内容：MDD、電磁弁×5、モーター×2
最終更新：2023/02/22
最終更新者:Ｓｈｉｏｎ Ｎｏｇｕｃｈｉ
SPDX-FileCopyrightText: 2023 Shion Noguchi
SPDX-License-Identifier:cc by 4.0
 */


#include "stm32f4xx.h"
#include "stm32f4xx_nucleo.h"
#include "sken_library/include.h"
#include "sken_mdd.h"

float COMMAND_MODE[4] = { 0, 0, 0, 0 };
float ENCODER_CONFIG[4] = { 8192, 8192, 8192, 8192 };
float DIAMETER_CONFIG[4] = { 101, 917, 0, 0 };  //タイヤの直径、旋回直径
float M1_PID_CONFIG[4] = { 10, 0.5, 0, 20 };    //ゲイン調整（P、I、D、時定数）
float M2_PID_CONFIG[4] = { 10, 0.5, 0, 20 };    //ゲイン調整（P、I、D、時定数）
float M3_PID_CONFIG[4] = { 10, 0.5, 0, 20 };    //ゲイン調整（P、I、D、時定数）
float M4_PID_CONFIG[4] = { 10, 0.5, 0, 20 };    //ゲイン調整（P、I、D、時定数）
float ROBOT_VOLCITY[4] = { 0.0, 0, 0, 0 };

PS3 ps3;
PS3_data ps3_data;
Gpio gpio;
Gpio denziben1;  //装填
Gpio denziben2;  //発射
Gpio denziben3;  //昇降
Gpio denziben4;
Gpio denziben5;
Gpio denziben6;
Gpio rimito1;
Gpio apin2;
Gpio bpin2;
Gpio pwmpin2;
RcPwm rc_pwm;
Encoder encoder;
Encoder encoder1;
float Lchi;
int ps3_flag;

float SP = 300;  //速度設定
float X = 8.0;   //x方向補正
float Y = 8.0;   //y方向補正

//LED
Gpio BLUE;
Gpio RED;
Gpio GREEN;
double redValue;
double greenValue;
double blueValue;



//装填
Gpio apin;
Gpio bpin;
Gpio pwmpin;

unsigned int time;
Pid pid_control;
double target;
double now;
double out1;
double e;
float deg;
float syuturyoku;
int count;
bool flag;
bool flag2;

SkenMdd mdd;

//Uart serial;


void func(void) {
  out1 = (pid_control.control(target, now, 1));
}

void soten() {

  now = ((double)(encoder.read()) / 8192.0 * 360.0);
  syuturyoku = out1;
  if (ps3_data.R2 == HIGH) {  //もし右下が押されたら
    flag = true;
    flag2 = true;
  }
  if (flag2 == true) {
     target = 45;//装填ターゲット
     flag2 = false;
     }
  if (flag == true) {
    if (out1 <= 0) {
      apin.write(LOW);
      bpin.write(HIGH);
      pwmpin.write(syuturyoku * -1);
    } else if (out1 > 0) {
      apin.write(HIGH);
      bpin.write(LOW);
      pwmpin.write(syuturyoku);
    }else if(out1 == 0){
    	 apin.write(HIGH);
    	 bpin.write(HIGH);
    	 pwmpin.write(0);
    }
    if (ps3_data.L2 == true) {
    	flag = false;
      }
  }
  if (flag == false) {
    encoder.reset();
    target = 0;
    out1 = 0;
    apin.write(HIGH);
    bpin.write(HIGH);
    pwmpin.write(0);
  }
}

void kakudo() {
  if (ps3_data.R1 == true) {  //もし右上ボタン押したら上がる
    apin2.write(HIGH);
    bpin2.write(LOW);
    pwmpin2.write(28);
  } else if (ps3_data.L1 == true) {  //もし左上ボタン押したら
    apin2.write(LOW);
    bpin2.write(HIGH);
    pwmpin2.write(10);
  }  else {
    apin2.write(LOW);
    bpin2.write(LOW);
    pwmpin2.write(0);
  }
  if (!rimito1.read()) {
  	apin2.write(HIGH);
  	bpin2.write(LOW);
  	pwmpin2.write(20);
    }
}
/*
void led(){
	redValue = 100;
	greenValue = 0;
	blueValue = 0;

	for(double i = 0; i < 100; i += 0.01)
	{
	redValue -= 0.01;
	greenValue += 0.01;

	RED.write(redValue);
	GREEN.write(greenValue);
	}

	redValue = 0;
	greenValue = 100;
	blueValue = 0;

	for(double i = 0; i < 100; i += 0.01)
	{
	greenValue -= 0.01;
	blueValue += 0.01;

	GREEN.write(greenValue);
	BLUE.write(blueValue);
	}

	redValue = 0;
	greenValue = 0;
	blueValue = 100;

	for(double i = 0; i < 100; i += 0.01)
	{

	blueValue -= 0.01;
	redValue += 0.01;

	BLUE.write(blueValue);
	RED.write(redValue);
	}
}*/


int main(void) {
  sken_system.init();

  gpio.init(A5, PWM_OUTPUT, TIMER2, CH1);
  denziben1.init(C5, OUTPUT);  //電磁弁ピン設定
  denziben2.init(B1, OUTPUT);
  denziben3.init(A12, OUTPUT);
  denziben4.init(A11, OUTPUT);
  denziben5.init(B8, OUTPUT);
  denziben6.init(B9, OUTPUT);
  rimito1.init(C2, INPUT_PULLUP);
  ps3.StartRecive();
  encoder.init(A0, A1, TIMER5);
  rc_pwm.init(B6, TIMER4, CH1);  //サーボピン設定
  rc_pwm.turn(0);                //サーボ初期角度


  //serial.init(A9,A10,SERIAL1,9600);//シリアル通信ピン設定

  mdd.init(C10, C11, SERIAL3);  //MDDピン設定
  mdd.tcp(MOTOR_COMMAND_MODE_SELECT, COMMAND_MODE, 10, 2000);
  mdd.tcp(ENCODER_RESOLUTION_CONFIG, ENCODER_CONFIG, 10, 2000);
  mdd.tcp(ROBOT_DIAMETER_CONFIG, DIAMETER_CONFIG, 10, 2000);
  mdd.tcp(M1_PID_GAIN_CONFIG, M1_PID_CONFIG, 10, 2000);
  mdd.tcp(M2_PID_GAIN_CONFIG, M2_PID_CONFIG, 10, 2000);
  mdd.tcp(M3_PID_GAIN_CONFIG, M3_PID_CONFIG, 10, 2000);
  mdd.tcp(M4_PID_GAIN_CONFIG, M4_PID_CONFIG, 10, 2000);

  //led
  BLUE.init(C9, PWM_OUTPUT, TIMER8, CH4, 980);
  RED.init(C8, PWM_OUTPUT, TIMER8, CH3, 980);
  GREEN.init(C6, PWM_OUTPUT, TIMER8, CH1, 980);
  redValue = 100;
  greenValue = 10;
  blueValue = 10;
  RED.write(redValue);
  GREEN.write(greenValue);
  BLUE.write(blueValue);

  //装填
  apin.init(C4, OUTPUT);
  bpin.init(B14, OUTPUT);
  pwmpin.init(B15, PWM_OUTPUT, TIMER12, CH2, 980);
  pid_control.setGain(4, 0.1, 0.01, 30);
  sken_system.addTimerInterruptFunc(func, 0, 1);
  apin.write(LOW);
  bpin.write(LOW);
  pwmpin.write(0);
  target = 0;
  flag = false;
  flag2 = false;

  //角度
  apin2.init(B5, OUTPUT);
  bpin2.init(B4, OUTPUT);
  pwmpin2.init(B10, PWM_OUTPUT, TIMER2, CH3, 980);

  while (true) {
    ps3.Getdata(&ps3_data);

    ROBOT_VOLCITY[0] = (float)ps3_data.LxPad / 63 * -SP * X;   //x方向
    ROBOT_VOLCITY[1] = (float)ps3_data.LyPad / 63 * -SP * Y;  //y方向
    ROBOT_VOLCITY[2] = (float)ps3_data.RxPad / 63 * SP;      //旋回時

    mdd.udp(MECANUM_MODE, ROBOT_VOLCITY);


    //serial.write(ps3_data.rx_data,8);//シリアル通信



    if (ROBOT_VOLCITY[0] == 0 && ROBOT_VOLCITY[1] == 0 && ROBOT_VOLCITY[2] == 0) {
      mdd.udp(PID_RESET_COMMAND, COMMAND_MODE);
    }

    soten();
    kakudo();


    if (ps3_data.Triangle == true) {  //もし三角ボタンが押されたら
      denziben1.write(HIGH);          //電磁弁1ON
    } else {
      denziben1.write(LOW);  //電磁弁1OFF
    }
    if (ps3_data.Circle == true) {  //もし丸ボタンが押されたら
      denziben2.write(HIGH);        //電磁弁2ON
    } else {
      denziben2.write(LOW);  //電磁弁2OFF
    }
    if (ps3_data.Start == true) {          //もしstartボタンが押されたら
      denziben3.write(HIGH);               //電磁弁3ON
    } else if (ps3_data.Select == true) {  //もしselectボタンが押されたら
      denziben3.write(LOW);                //電磁弁3OFF
    }
    if (ps3_data.Up == true) {  //もし上ボタンが押されたら
      denziben4.write(HIGH);    //電磁弁4ON
    } else {
      denziben4.write(LOW);  //電磁弁4OFF
    }
    if (ps3_data.Cross == true) {  //もし×ボタンが押されたら
      denziben5.write(HIGH);        //電磁弁5ON
    } else {
      denziben5.write(LOW);  //電磁弁5OFF
    }
    if (ps3_data.Square == true) {  //もし四角ボタンが押されたら
      denziben6.write(HIGH);        //電磁弁6ON
    } else {
      denziben6.write(LOW);  //電磁弁6OFF
    }
  }
}
