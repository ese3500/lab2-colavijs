
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

// 4.8 *************************************************************
volatile int edgeDetected = 0;
volatile int lightOn = 0;
volatile int addSpace = 0;
volatile unsigned long period = 0;
volatile unsigned long edge = 0;
volatile int timerOverflows = 0;
int translatedL[5] = {2, 2, 2, 2, 2};
int letterIndex = 0;

void Initialize()
{
	cli();
	
	UART_init(BAUD_PRESCALER);

	//sets pin 0 as input
	DDRB &= ~(1<<DDB0);
	
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);
	
	PORTB &= ~(1<<PORTB1);
	PORTB &= ~(1<<PORTB2);
	PORTB &= ~(1<<PORTB3);
	
	// Enable clock (/8; 1t = 1/2M sec)
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	// Detect rising edge for first button press
	TCCR1B |= (1<<ICES1);
	
	// Clear input capture flag
	TIFR1 |= (1<<ICF1);
	
	// Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	// enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	
	//Sets the clock to normal
	TCCR1B &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);

	sei();
}

ISR(TIMER1_CAPT_vect)
{
	if (!edgeDetected) {
		edgeDetected = 1;
		TIFR1 |= (1 << ICF1);
		edge = ICR1;
		TCCR1B &= ~(1 << ICES1);
		timerOverflows = 0;
	} else {
		edgeDetected = 0;
		TIFR1 |= (1 << ICF1);
		TCCR1B |= (1 << ICES1);
		period = ICR1 - edge + timerOverflows * 65536;
		lightOn = 1;
		timerOverflows = 0;
	}
	TIFR1 |= (1<<ICF1);
}

ISR(TIMER1_OVF_vect)
{
	timerOverflows++;
	if (timerOverflows > 16) {
		addSpace = 1;
		timerOverflows = 0;
	}
	TIFR1 |= (1<<ICF1);
}

int main(void)
{
	Initialize();
	
	while (1) {
		char String[30];
		if (addSpace || letterIndex > 4)
		{
			if (letterIndex == 1) {
				if (translatedL[0] == 1) {
					sprintf(String,"T");
					UART_putstring(String);
				} else if (translatedL[0] == 0) {
					sprintf(String,"E");
					UART_putstring(String);
				}
			} else if (letterIndex == 2) {
				if (translatedL[0] == 1) {
					if (translatedL[1] == 1) {
						sprintf(String,"M");
						UART_putstring(String);
					} else if (translatedL[1] == 0) {
						sprintf(String,"N");
						UART_putstring(String);
					}
					
				} else if (translatedL[0] == 0) {
					if (translatedL[1] == 0) {
						sprintf(String,"I");
						UART_putstring(String);
					} else if (translatedL[1] == 1) {
						sprintf(String,"A");
						UART_putstring(String);
					}
				}
			} else if (letterIndex == 3) {
				if (translatedL[0] == 1) {
					if (translatedL[1] == 1) {
						if (translatedL[2] == 1) {
							sprintf(String,"O");
							UART_putstring(String);
						} else if (translatedL[2] == 0) {
							sprintf(String,"G");
							UART_putstring(String);
						}
					} else if (translatedL[1] == 0) {
						if (translatedL[2] == 1) {
							sprintf(String,"K");
							UART_putstring(String);
						} else if (translatedL[1] == 0) {
							sprintf(String,"D");
							UART_putstring(String);
						}
					}
				} else if (translatedL[0] == 0) {
					if (translatedL[1] == 1) {
						if (translatedL[2] == 1) {
							sprintf(String,"W");
							UART_putstring(String);
						} else if (translatedL[2] == 0) {
							sprintf(String,"R");
							UART_putstring(String);
						}
					} else if (translatedL[1] == 0) {
						if (translatedL[2] == 1) {
							sprintf(String,"U");
							UART_putstring(String);
						} else if (translatedL[2] == 0) {
							sprintf(String,"S");
							UART_putstring(String);
						}
					}
				}
			} else if (letterIndex == 4) {
				if (translatedL[0] == 1 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 0) {
					sprintf(String,"B");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 0 && translatedL[2] == 1 && translatedL[3] == 0) {
					sprintf(String,"C");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 1 && translatedL[3] == 0) {
					sprintf(String,"F");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 0) {
					sprintf(String,"H");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 1) {
					sprintf(String,"J");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 1 && translatedL[2] == 0 && translatedL[3] == 0) {
					sprintf(String,"L");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 0) {
					sprintf(String,"P");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 1 && translatedL[2] == 0 && translatedL[3] == 1) {
					sprintf(String,"Q");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 1) {
					sprintf(String,"V");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 1) {
					sprintf(String,"X");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 0 && translatedL[2] == 1 && translatedL[3] == 1) {
					sprintf(String,"Y");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 1 && translatedL[2] == 0 && translatedL[3] == 0) {
					sprintf(String,"Z");
					UART_putstring(String);
				}
			} else if (letterIndex == 5) {
				if (translatedL[0] == 0 && translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 1 && translatedL[4] == 1) {
					sprintf(String,"1");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 1 && translatedL[3] == 1 && translatedL[4] == 1) {
					sprintf(String,"2");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 1 && translatedL[4] == 1) {
					sprintf(String,"3");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 0 && translatedL[4] == 1) {
					sprintf(String,"4");
					UART_putstring(String);
				} else if (translatedL[0] == 0 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 0 && translatedL[4] == 0) {
					sprintf(String,"5");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 0 && translatedL[2] == 0 && translatedL[3] == 0 && translatedL[4] == 0) {
					sprintf(String,"6");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 1 && translatedL[2] == 0 && translatedL[3] == 0 && translatedL[4] == 0) {
					sprintf(String,"7");
					UART_putstring(String);
				} else if (translatedL[0] == 1&& translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 0 && translatedL[4] == 0) {
					sprintf(String,"8");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 1 && translatedL[4] == 0) {
					sprintf(String,"9");
					UART_putstring(String);
				} else if (translatedL[0] == 1 && translatedL[1] == 1 && translatedL[2] == 1 && translatedL[3] == 1 && translatedL[4] == 1) {
					sprintf(String,"0");
					UART_putstring(String);
				}
			}
			addSpace = 0;
			memset(String, 0, sizeof(String));
			translatedL[0] = 2;
			translatedL[1] = 2;
			translatedL[2] = 2;
			translatedL[3] = 2;
			translatedL[4] = 2;
			letterIndex = 0;
		}
		if (period > 60000 && period < 533333) {
			if (lightOn) {
				lightOn = 0;
				translatedL[letterIndex] = 0;
				letterIndex++;
				PORTB |= (1 << PORTB1); // Turn the LED on
				_delay_ms(50); // Wait for 50 milliseconds
				PORTB &= ~(1 << PORTB1); // Turn the LED off
			}
		} else if (period > 533333 && period < 1066666) {
			if (lightOn) {
				lightOn = 0;
				translatedL[letterIndex] = 1;
				letterIndex++;
				PORTB |= (1 << PORTB2); // Turn the LED on
				_delay_ms(50); // Wait for 50 milliseconds
				PORTB &= ~(1 << PORTB2); // Turn the LED off
			}
		}
	}
	_delay_ms(100);
}
// 4.8 *************************************************************

// 3.6 *************************************************************
/*volatile int edge_detected = 0;
volatile unsigned long period = 0;
volatile unsigned long edge = 0;
volatile int timerOverflows = 0;

void Initialize()
{	
	cli();
	
	UART_init(BAUD_PRESCALER);

	//sets pin 0 as input
	DDRB &= ~(1<<DDB0);
	
	
	// Enable clock (/8; 1t = 1/2M sec)
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	// Detect rising edge for first button press
	TCCR1B |= (1<<ICES1);
	
	// Clear input capture flag
	TIFR1 |= (1<<ICF1);
	
	// Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	// enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	
	//Sets the clock to normal
	TCCR1B &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);

	sei();
}

ISR(TIMER1_CAPT_vect)
{
	if (!edge_detected) {
		edge_detected = 1;
		TIFR1 |= (1 << ICF1);
		edge = ICR1;
		TCCR1B &= ~(1 << ICES1);
		timerOverflows = 0;
	} else {
		edge_detected = 0;
		TIFR1 |= (1 << ICF1);
		TCCR1B |= (1 << ICES1);
		period = ICR1 - edge + timerOverflows * 65536;
	}
	TIFR1 |= (1<<ICF1);
}

ISR(TIMER1_OVF_vect)
{
	timerOverflows++;
	TIFR1 |= (1<<ICF1);
}

int main(void)
{
	Initialize();
	
	while (1) {
		char String[10];
		if (period > 60000 && period < 400000) {
			sprintf(String, "dot\n");
		} else if (period > 400000 && period < 800000) {
			sprintf(String, "dash\n");
		} else if (period > 800000) {
			sprintf(String, "space\n");
		}
		UART_putstring(String);
		_delay_ms(3000);
	}
}*/
// 3.6 *************************************************************

// 2.5 *************************************************************
/*void Initialize()
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
}*/
// 2.5 *************************************************************

// 1.3 *************************************************************
/*void Initialize()
{
	cli();

	//	sets pins 9 to 12 as outputs
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);
	DDRB |= (1<<DDB4);
	//	sets pin 7 as output
	DDRD |= (0<<DDD7);

	//  set pin 9 to high and the rest on that register to low
	PORTB |= (1<<PORTB1);
	//  Enable pull-up resistor on pin 7
	PORTD |= (1<<PORTD7);

	sei();
}

int main(void)
{
	Initialize();
	
	int count = 1;
	while (1) {
		if ((PIND & (1 << PORTD7)) == 0) {
			_delay_ms(500); // Wait 500 ms 
			if ((PIND & (1 << PORTD7)) == 0) {
				count++;
				if (count > 4) {
					count = 1;
				}
				PORTB |= (1<<count); // turn on the next light
				PORTB &= (1<<count); // turn off the rest
			}
		}
	}
}*/
// 1.3 *************************************************************

// 1.2 *************************************************************
/*void Initialize() 
{
	cli();

//	sets pins 9 to 12 as outputs
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);
	DDRB |= (1<<DDB4);
//	sets pin 7 as input
	DDRD |= (0<<DDD7);

//  set pins 9 to 12 as high
//	PORTB |= (1<<PORTB1);
//	PORTB |= (1<<PORTB2);
//	PORTB |= (1<<PORTB3);
//	PORTB |= (1<<PORTB4);
//  Enable pull-up resistor on pin 7
	PORTD |= (1<<PORTD7);

	sei();
}

int main(void)
{
	Initialize();
	 
    while (1) {
		if ((PIND & (1 << PORTD7)) == 0) {
			PORTB |= (1<<PORTB1);
		} else {
			PORTB &= (0<<PORTB1);
		}
    }
}*/
// 1.2 *************************************************************

