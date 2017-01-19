#include <reg51.h>

void delay();
void init();

#define pwm_width P0;
bit pwm_flag = 0;
unsigned char left_motor_mode, right_motor_mode; // controls the direction of each motor's state (forward, nautral, or reverse). The two values will be ORed and given to motor driver.

void main() {
	init();
	
	while (1) {
		delay();
		//delay();
		//delay();
		//delay();
		//P1 = P1<<1;
		//if (P1==0x00) P1 = 0x01;
	}
}

void init() {
	TMOD = 0x01;	// Timer1 Gate:0 C/T:0 Mode:00 (13-bit timer) // Timer0 Gate:0 C/T:0 Mode:01 (16-bit timer)
	EA = 1; 		// Global Interrupt Enable
	ES = 1;			// Enable Serial Interrupt
	ET1 = 1;		// Enable Timer 1 Interrupt
	TR1 = 1; 		// Turn on timer0 used in delay()
	P2 = 0x01;		// LED ON
}
void delay() {  // 25ms delay 
				// (12 crystal cycle = 1 machine cycle)
				// (12/11.0599MHz)(2^16 - x)=25ms
				// x = 2^16 - 25ms*11.0592MHz/12 = 42496 = A600H
	TL0=0x00;	// load counting value
	TH0=0xA6;	// load counting value 
	TR0=1;		// turn on timer0
	while (TF0==0); // wait for overflow flag
	TR0=0;		// turn off timer0 on overflow
	TF0=0;		// clear overflow flag (won't be reset by hardware)
}

void timer1() interrupt 3 { // implentation of http://www.8051projects.net/wiki/Pulse_Width_Modulation
	if (!pwm_flag) {
		pwm_flag = 1;
		P2 = 0x01;				// LED ON
		P1 = left_motor_mode | right_motor_mode; // MOTORS ON	ports 0,1 = right; ports 2,3 = left // 0A = forward 05 = backward
		TH1 = pwm_width;
	} else {
		pwm_flag = 0;
		P2 = 0x00;				// LED OFF
		P1 = 0x00;				// MOTORS OFF
		TH1 = 255 - pwm_width;
	}
	TF1 = 0;	// clear overflow flag (won't be reset by hardware)
}

void serial() interrupt 4 {
	/*
	if (c=='0') {
		left_motor_mode = 0x00;
		right_motor_mode = 0x00;
	}
	elseif (c=='5') {
		left_motor_mode = 0x01;
		right_motor_mode = 0x04;
	}
	else if (c=='A') {
		left_motor_mode = 0x02;
		right_motor_mode = 0x08;
	}
	else if (c=='9') {
		left_motor_mode = 0x01;
		right_motor_mode = 0x08;
	}
	else if (c=='6') {
		left_motor_mode = 0x02;
		right_motor_mode = 0x01;
	}
	else if (c=='4') {
		left_motor_mode = 0x00;
		right_motor_mode = 0x04;
	}
	else if (c=='8') {
		left_motor_mode = 0x00;
		right_motor_mode = 0x08;
	}
	else if (c=='1') {
		left_motor_mode = 0x01;
		right_motor_mode = 0x00;
	}
	else if (c=='2') {
		left_motor_mode = 0x02;
		right_motor_mode = 0x00;
	}
	
	*/
}