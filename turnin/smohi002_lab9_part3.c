/*	Author: sana
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "../header/timer.h"
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

enum States_off {start, off, on, onBeat, wait} state;

unsigned char A;
//EEFGGFEDCCDEEDD
unsigned short notes[15] = {0x149, 0x149, 0x15D, 0x188, 0x188, 0x15D, 0x149, 0x125, 0x105, 0x105, 0x125, 0x149, 0x149, 0x125, 0x125};
//	0x105, 0x125, 0x149, 0x15D, 0x188, 0x1B8, 0x1ED, 0x20B
unsigned char time[15] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 4};
unsigned char i = 0x00;
unsigned char t = 0x00;

void Tick() {

	switch(state) {
		case start:
			state = wait;
			set_PWM(0x00);
			i = 0x00;
			break;
		case off:
			if (A) state = on;
			else state = off;
			break;
		case on:
			state = onBeat;
			break;
		case onBeat:
			if (t <= time[i]) state = onBeat;
			else if (i > 15 && !A) {state = off; set_PWM(0x00);}
			else if (i > 15 && A) {state = wait; set_PWM(0x00);}
			else {state = on; i++; set_PWM(0x00);}
			break;
		case wait:
			if (A) state = wait;
			else state = off;
			break;
		default:
		    	state = start;	
			break;
	}

	switch(state) {
		case start: break;
		case off:
			set_PWM(0x00);
			i = 0x00;
			t = 0x00;
			break;
		case on:
			set_PWM(notes[i]);
			break;
		case onBeat:
			t++;
			break;
		case wait:	
			set_PWM(0x00);
			break;
		default: break;

	}
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	state = start;
	PWM_on();
    
	TimerSet(250);
	TimerOn();
	
	while (1) {
		A = ~PINA & 0x01;
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
    PWM_off();
    return 1;
}
