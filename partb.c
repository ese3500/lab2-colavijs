/*
 * Lab2Morsecolavijs.c
 *
 * Created: 1/27/2023 11:51:04 AM
 * Author : johnc
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>

#include "uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

void Initialize()
{
	cli();
	
	// sets pin 13 as output
	DDRB |= (1<<DDB5);
	// sets pin 13 as LOW
	PORTB &= ~(1<<PORTB0);

	//sets pin 7 as input
	DDRB &= ~(1<<DDB0);

	// Enable clock (/8; 1t = 1/2M sec)
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	//Sets the clock to normal
	TCCR1B &= ~(1<<WGM10);
	TCCR1B &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
	// Detect rising edge for first button press
	TCCR1B |= (1<<ICES1);
	
	// Clear input capture flag
	TIFR1 |= (1<<ICF1);
	
	// Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);

	sei();
}

char String[25];

ISR(TIMER1_CAPT_vect)
{
	if ((PINB & (1 << PINB0))) { // Check if the button was pressed
		PORTB |= (1<<PORTB5); // turn on the internal LED
		TCCR1B &= ~(1<<ICES1);
		} else {
		PORTB &= ~(1<<PORTB5);
		TCCR1B |= (1<<ICES1);
	}
}

int main(void)
{
	Initialize();
	
	while (1) {
	}
}