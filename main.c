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
	}
}

void init() {
	TMOD = 0x01;	// Timer1 Gate:0 C/T:0 Mode:00 (13-bit timer) // Timer0 Gate:0 C/T:0 Mode:01 (16-bit timer)
	TR1  = 1; 		// Turn on Timer 1
	SCON = 0x09;	// Serial Mode 2 (9-bit UART) Baud Rate: OSC/64
	IE   = 0x98; 	// Enable Global, Serial, Timer 1 Interrupt
	P2   = 0x01;	// LED ON
	left_motor_mode = 0x00;		// Default Motor Neutral
	right_motor_mode = 0x00;
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
	} else {
		pwm_flag = 0;
		P2 = 0x00;				// LED OFF
		P1 = 0x00;				// MOTORS OFF
		TH1 = 255 - pwm_width;
	}
	TF1 = 0;	// clear overflow flag (won't be reset by hardware)
}

void serial() interrupt 4 {
	char c;
	IE = 0x00; 	// Temporarily disable other interrupts
	while (RI==0);
	c = SBUF;
//	if (c=='0') { 					// Neutral
//		left_motor_mode = 0x00;
//		right_motor_mode = 0x00;
//	}
	if (c=='5') { 					// Forward
		left_motor_mode = 0x01;
		right_motor_mode = 0x04;
	}
	else if (c=='A') {				// Reverse
		left_motor_mode = 0x02;
		right_motor_mode = 0x08;
	}
	else if (c=='9') {				// Tank Left
		left_motor_mode = 0x01;
		right_motor_mode = 0x08;
	}
	else if (c=='6') {				// Tank Right
		left_motor_mode = 0x02;
		right_motor_mode = 0x01;
	}
//	else if (c=='4') {				// Right Forward
//		left_motor_mode = 0x00;
//		right_motor_mode = 0x04;
//	}
//	else if (c=='8') {				// Right Backward
//		left_motor_mode = 0x00;
//		right_motor_mode = 0x08;
//	}
//	else if (c=='1') {				// Left Forward
//		left_motor_mode = 0x01;	
//		right_motor_mode = 0x00;
//	}
//	else if (c=='2') {				// Left Reverse
//		left_motor_mode = 0x02;
//		right_motor_mode = 0x00;
	IE=0x98;	// change Interrupts back to normal
}
