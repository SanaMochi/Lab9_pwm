/*	Author: sana
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
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

enum States_off {start, wait, on, onFall, inc, incFall, dec, decFall, off, offFall/*, incOff, incFallOff, decOff, decFallOff*/} state;

unsigned char A;
unsigned short notes[8] = {0x105, 0x125, 0x149, 0x15D, 0x188, 0x1B8, 0x1ED, 0x20B};
unsigned char i = 0x00;
unsigned char pwr;

void Tick() {

	switch(state) {
		case start:
			state = wait;
			set_PWM(0x00);
			i = 0x00;
			pwr = 0x00;
			break;
		case wait:
			if (A == 0x01 && !pwr) state = on;
			else if (A == 0x01 && pwr) state = off;
			else if (A == 0x02 && pwr) state = inc;
			else if (A == 0x04 && pwr) state = dec;
//			else if (A == 0x02 && !pwr) state = incOff;
//                      else if (A == 0x04 && !pwr) state = decOff;
			else state = wait; 
			break;
		case on:
		       	state = onFall;	
			break;
		case onFall:
			if (A) state = onFall;
			else state = wait;
		case inc:
			state = incFall;
			break;
		case incFall:
			if (A == 0x02) state = incFall;
			else state = wait;
			break;
		case dec:
			state = decFall;
			break;
		case decFall:
			if (A == 0x04) state = decFall;
			else state = wait;
			break;

		case off:
                        state = offFall;
                        break;
                case offFall:
                        if (A) state = offFall;
                        else state = wait;
                	break;
/*		case incOff:
                        state = incFall;
                        break;
                case incFallOff:
                        if (A == 0x02) state = incFall;
                        else state = wait;
                        break;
                case decOff:
                        state = decFall;
                        break;
                case decFallOff:
                        if (A == 0x04) state = decFall;
                        else state = wait;
                        break;
*/		default:
		    	state = start;	
			break;
	}

	switch(state) {
		case start: break;
                case wait:  break;
		case on:
			set_PWM(notes[i]);
			pwr = 0x01;
			break;
		case onFall: break;
		case inc:
			if (i < 7) set_PWM(notes[++i]);
                        break;
		case incFall:
			set_PWM(notes[i]);
			break;
                case dec:
			if (i > 0) set_PWM(notes[--i]);
                        break;
		case decFall:
			set_PWM(notes[i]);
			break;

		case off:
                        set_PWM(0x00);
			pwr = 0x00;
                        break;
                case offFall: break;
/*                case incOff:
                        if (i < 7) ++i;
                        break;
                case incFallOff: break;
                case decOff:
                        if (i > 0) --i;
                        break;
                case decFallOff: break;
*/
		default: break;

	}
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	state = start;
	PWM_on();
    
	TimerSet(50);
	TimerOn();
	
	while (1) {
		A = ~PINA & 0x07;
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
    PWM_off();
    return 1;
}
