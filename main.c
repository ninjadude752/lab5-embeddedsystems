/*
 * Lab5Embbeded.c
 *
 * Created: 4/10/2023 2:50:09 PM
 * Author : Xinyi Yuan
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL	// 16 MHz clock speed
#endif
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define slaveAddr  0b01011000		// slave addr
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "twimaster.c"
//#include "i2cmaster.h"

/* function declaration */
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

int main()
{	
	//DDRC = 0b00110000;		// configure PC4(SDA), PC5(SCL) as output, other pins as inputs(potentiometer connected to PC0)
	USART_Init(MYUBRR);
	unsigned char data[] = "v = ";
	unsigned char newLine[] = "V \n";
	unsigned char line[] = "\n";
	unsigned char str[20];
	unsigned char readInArr[10];
	unsigned char dacChannel[] = "DAC channel ";
	unsigned char v[5];			// voltage value
	unsigned char f[3];			// frequency value
	unsigned char r[4];
	ADCSRA = 0b10000111;		// enable ADC, set pre-scaler
	ADMUX = 0b01000000;			// AVcc with external capacitor at AREF pin
	float high;
	float Vin;
	

	i2c_init();
	
	while(1){
		// ADC
		int i = 0;
		unsigned char readIn = USART_Receive();
		while (readIn != '\n'){
			readInArr[i] = readIn;
			i = i + 1;
			readIn = USART_Receive();
		}
		
		if (readInArr[0] == 'G') {
			ADCSRA = 0b11000111;
			while (ADCSRA == 0b11000111);	// wait until the second bit is low
			high = ADC;
			Vin = high * 5 / 1024;
			dtostrf(Vin, 2, 3, str);		// adc value, width, decimal place, char array
		
			int i = 0;
			while (data[i] != 0) {			// print "v = "
				USART_Transmit(data[i]);
				i = i + 1;
			}
			i = 0;
			while (str[i] != 0) {			// Vin value
				USART_Transmit(str[i]);
				i = i + 1;
			}

			i = 0;
			while (newLine[i] != 0) {		// print "V \n"
				USART_Transmit(newLine[i]);
				i = i + 1;
			}
			strcpy(str, "\0");
			strcpy(readInArr, "\0");		
		}
		
		// command for S, set DAC output voltage
		else if (readInArr[0] == 'S') {
			if (readInArr[1] == ',') {
				if (readInArr[2] == '1' || readInArr[2] == '0') {
					if (readInArr[3] == ',') {
						unsigned char temp = readInArr[2];
						strncat(dacChannel, &temp, 1);
						// printing
						int i = 0;
						while (dacChannel[i] != 0) {			// print DAC 1 or DAC 0
							USART_Transmit(dacChannel[i]);
							i = i + 1;
						}
						unsigned char space = ' ';
						USART_Transmit(space);
						int count;
						for (count = 0; count < 4; ++count) {
							USART_Transmit(readInArr[count+4]);
							v[count] = readInArr[count+4];
						}
						
						i = 0;
						while (line[i] != 0) {					// print new line
							USART_Transmit(line[i]);
							i = i + 1;
						}
						
						
						// I2C: setting output voltage
						// send start condition
						int value = atof(v);
						value = value / 5 * 256;
						i2c_start(slaveAddr);
						if (readInArr == '0') {
							// command byte
							i2c_write(0x00);			// set DAC0 output
						}
						else {
							i2c_write(0x01);			// set DAC1 output
						}
						// output byte
						i2c_write(0x00);			
						i2c_stop();
						
						strcpy(v, "");							// reset v
						strcpy(readInArr, "\0");
						strcpy(dacChannel, "DAC channel ");		// reset dacChannel
						
					}
					
				}
			}
		}
		
		// command for W, generate a sine wave on DAC output
		else if (readInArr[0] == 'W') {
			if (readInArr[1] == ',') {
				if (readInArr[2] == '1' || readInArr[2] == '0') {
					if (readInArr[3] == ',') {
						f[0] = readInArr[4];
						f[1] = readInArr[5];
						if (readInArr[6] == ',') {
							int i = 0;
							while (readInArr[i] != 0) {					// print new line
								USART_Transmit(readInArr[i]);
								i = i + 1;
							}
							
							i = 0;
							while (line[i] != 0) {					// print new line
								USART_Transmit(line[i]);
								i = i + 1;
							}
							
							strcpy(readInArr, "");

						}
					}
				}
			}
		}
		
		
		
		
	}	
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


	
	

	
