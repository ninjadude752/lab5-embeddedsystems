/*
 * Lab5Embbeded.c
 *
 * Created: 4/10/2023 2:50:09 PM
 * Author : xyuan13
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL	// 16 MHz clock speed
#endif
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{	
	DDRC = 0b00110000;		// configure PC4(SDA), PC5(SCL) as output, other pins as inputs(potentiometer connected to PC0)
	USART_Init(MYUBRR);
	return 0;
}
 void USART_Init(unsigned int ubrr) {
	/*set baud rate*/
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	/* Set frame format: 8 data, 2 stop bit */
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
}
	
	
	/*
	unsigned char c;
	char str[25];
	int adH, adL, dac;
	int i;
	
	// get the chars the user types at the PC keyboard into a char string
	usart_prints("Please type 4 characters!");
	for (i = 0; i < 4-1; i ++) {
		c = usart_getc();	// Get character
		usart_putc(c);		// Echo it back
		str[i] = c;
	}
	str[i] = '\0';
	*/
 


	
