
#include <reg51.h>

void delay();
void init();
void sendChar(char c);

#define pwm_width P0;
bit pwm_flag = 0;
unsigned char left_motor_mode, right_motor_mode; // controls the direction of each motor's state (forward, nautral, or reverse). The two values will be ORed and given to motor driver.

void main() {
	init();
	sendChar('s');
	sendChar('t');
	sendChar('a');
	sendChar('r');
	sendChar('t');
	while (1);
}

void init() {
	TMOD = 0x20;	// Timer1 Gate:0 C/T:0 Mode:10 (8-bit auto-reload) // Timer0 Gate:0 C/T:0 Mode:00 (13-bit timer)
	TH1  = 0xFD;	// TH1 = 256 - ((11059000 / 384) / 9600) = 232 = 0xFD
	TR1  = 1; 		// Turn on Timer 1 used for Baud Rate
	TR0  = 1;		// Turn on Timer 0 used for PWM
	
	SCON = 0x50;	// Serial Mode 1 (8-bit UART) and Receiver Enabled
	IE   = 0x98; 	// Enable Global, Serial, Timer 0 Interrupt
	P2   = 0x01;	// LED ON
	
	left_motor_mode = 0x02;		// Default Motor Neutral
	right_motor_mode = 0x01;
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
		TH0 = 255 - pwm_width;
	}
	TF0 = 0;	// clear overflow flag (won't be reset by hardware)
}

void serial() interrupt 4 { // http://www.8052.com/tutser.phtml
	char c;
	ES = 0; //IE = 0x00; 	// Temporarily disable serial interrupt // Temporarily disable other interrupts to prevent recursion
	c = SBUF;
	if (c=='5') { 					// Forward
		P2   = 0x05;
		left_motor_mode = 0x01;
		right_motor_mode = 0x04;
		sendChar('5');
	}
	else if(c==0x0d) {                               
		SBUF='A';                 //Sending back "ACK" as   Acknowledgement 
		while(TI==0);
		TI=0;
		SBUF='C';
		while(TI==0);
		TI=0;
		SBUF='K';
		while(TI==0);
		TI=0;
	}
	else if (c=='A') {				// Reverse
		P2   = 0x0A;
		left_motor_mode = 0x02;
		right_motor_mode = 0x08;
		sendChar('A');
	}
	else if (c=='9') {				// Tank Left
		P2   = 0x09;
		left_motor_mode = 0x01;
		right_motor_mode = 0x08;
		sendChar('9');
	}
	else if (c=='6') {				// Tank Right
		P2   = 0x06;
		left_motor_mode = 0x02;
		right_motor_mode = 0x01;
		sendChar('6');
	}
	RI=0;			
	ES = 1; //IE=0x98;	// change Interrupts back to normal
}

void sendChar(char c) {
	SBUF=c;
	while(!TI);
	TI=0;
}
