/*
 * Lab5.c
 *
 * Created: 4/10/2023 2:50:09 PM
 * Author : xyuan13
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL	// 16 MHz clock speed
#endif
#include <avr/io.h>

int main(void)
{
	DDRC = 0b00110000;		// configure PC4(SDA), PC5(SCL) as output, other pins as inputs
	
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
 
}

	
