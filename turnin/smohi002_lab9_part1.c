/*	Author: sana
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {

	static double current_frequency;

	if (frequency != current_frequency) {
		if (!frequency) TCCR3B &= 0x08;
		else TCCR3B |= 0x03;

		if (frequency < 0.954) OCR3A = 0xFFFF;
		else if (frequency > 31250) OCR3A = 0x0000;
		else OCR3A = (short) (8000000 / (128 * frequency)) - 1;

		TCNT3 = 0;
		current_frequency = frequency;

	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3A = 0x00;
}


enum States {start, wait, c4, d4, e4} state;

unsigned char A;

void Tick() {

	switch(state) {
		case start:
			state = wait;
			break;
		case wait:
			if (A == 0x01) state = c4;
			else if (A == 0x02) state = d4;
			else if (A == 0x04) state = e4;
			else state = wait; 
			break;
		case c4: 
			if (A == 0x01) state = c4;
			else state = wait;
			break;
		case d4:
			if (A == 0x02) state = d4;
			else state = wait;
			break;
		case e4:
			if (A == 0x04) state = e4;
			else state = wait;
			break;
		default:
		    	state = start;	
			break;
	}

	switch(state) {
		case start: break;
                case wait:
			set_PWM(0x00);
                        break;
                case c4:
			set_PWM(0x105);
                        break;
                case d4:
			set_PWM(0x125);
                        break;
                case e4:
			set_PWM(0x149);
                        break;
		default: break;

	}
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	state = start;
	PWM_on();
    
	while (1) {
		A = ~PINA & 0x07;
//		PWM_on();
		Tick();
//		set_PWM(0x149);
	}
    PWM_off();
    return 1;
}
