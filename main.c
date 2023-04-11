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

void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

int main()
{	
	DDRC = 0b00110000;		// configure PC4(SDA), PC5(SCL) as output, other pins as inputs(potentiometer connected to PC0)
	USART_Init(MYUBRR);
	unsigned char data[] = "hello world\n";
	
	ADCSRA = 0b10000111;		// enable ADC, set pre-scaler
	ADMUX = 0b01000000;			// AVcc with external capacitor at AREF pin
	ADCSRA = 0b11000111;
	while (ADCSRA == 0b11000111);	// wait until the second bit is low
	float high = ADCH;
	float low = ADCL;
	
	while(1){
		int i = 0;
		while (data[i] != 0) {
			USART_Transmit(data[i]);
			i = i + 1;
		}
		USART_Receive();
	}
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

void USART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1<<UDRE0))) ;
	UDR0 = data;
}

unsigned char USART_Receive(void) {
	while (!(UCSR0A & (1<<RXC0))) ;
	
	return UDR0;
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
 


	
