/*
 * Lab5Embbeded.c
 *
 * Created: 4/10/2023 2:50:09 PM
 * Author : Xinyi Yuan
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL	// 16 MHz clock speed
#endif
#define __DELAY_BACKWARD_COMPATIBLE__

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
	unsigned char hz[] = " Hz on DAC channel ";
	unsigned char v[5];			// voltage value
	unsigned char f[2];			// frequency value
	unsigned char r[4];
	int sinWave[] = {128, 141, 153, 165, 177, 188, 199, 209, 219, 227, 234, 241, 246, 250, 254, 255, 255, 255, 254, 250, 246, 241, 234, 227, 219, 209, 199, 188, 177, 165, 153, 141, 128, 115,
					 103, 91, 79, 68, 57, 47, 37, 29, 22, 15, 10, 6, 2, 1, 0, 1, 2, 6, 10, 15, 22, 29, 37, 47, 57, 68, 79, 91, 103, 115};
	int delayTable[] = {1250,1150,1050,950,850,750,650,550,450,350,250,150};
	ADCSRA = 0b10000111;		// enable ADC, set pre-scaler
	ADMUX = 0b01000000;			// AVcc with external capacitor at AREF pin
	float high;
	float Vin;
	int delay = 250;
	int select = 0;

	i2c_init();
	
	while(1){
		// reset readInArr
		for (int j = 0; j < 10; ++j) {
			readInArr[j] = "\n";
		}
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
			strcpy(str, "\n");
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
						for (i = 0; i < strlen(dacChannel); ++i) {
							USART_Transmit(dacChannel[i]);
						}
						
						unsigned char space = ' ';
						USART_Transmit(space);
						int count;
						for (count = 0; count < 4; ++count) {
							USART_Transmit(readInArr[count+4]);
							v[count] = readInArr[count+4];
						}
						
						
						for (i = 0; i < strlen(line); ++i) {
							USART_Transmit(line[i]);
						}
						
						
						// I2C: setting output voltage
						// send start condition
						double value = atof(v);
						value = value / 5 * 256;
						i2c_start(slaveAddr);
						if (readInArr[2] == '0') {
							// command byte
							i2c_write(0x00);			// set DAC0 output
							//USART_Transmit(readInArr[2]);
						}
						else if (readInArr[2] == '1') {
							i2c_write(0x01);			// set DAC1 output
						}
						// output byte
						i2c_write(value);			
						i2c_stop();
						
						for (i = 0; i < 5; ++ i) {
							v[i] = '\n';
						}
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
						if (readInArr[6] == ',') {
							f[0] = readInArr[4];
							f[1] = readInArr[5];
							
							int i = 0;						
							unsigned char generate[] = "Generating ";
							for (i = 0; i < strlen(generate); ++i) {
								USART_Transmit(generate[i]);
							}
							/*
							i = 7;
							int j = 0;
							while (readInArr[i] != 0) {					// number of waveform cycles 
								r[j] = readInArr[i];
								USART_Transmit(readInArr[i]);
								i = i + 1;
								j = j + 1;
							}
							*/
							
							for (i = 7; i < 10; ++i) {
								if(readInArr[i] == 'v'){
									break;
								}
								else{
									r[i-7] = readInArr[i];
									USART_Transmit(readInArr[i]);	
								}
								
							}
							
							int numWave = (int)atof(r);
						
							unsigned char sineWave[] = " sine wave cycles with f = ";						
							for (i = 0; i < strlen(sineWave); ++i) {
								USART_Transmit(sineWave[i]);
							}
							
							USART_Transmit(f[0]);					// frequency of the waveform
							USART_Transmit(f[1]);
							float frequency = atof(f);
							
							for (i = 0; i < strlen(hz); ++i) {
								USART_Transmit(hz[i]);
							}
										
							
							USART_Transmit(readInArr[2]);			// channel number
							
							for (i = 0; i < strlen(line); ++i) {	// print new line
								USART_Transmit(line[i]);
							}
							
							
							for (int j = 0; j < numWave; ++j) {
								for (i = 0; i < 64; ++i) {
									// I2C: generating sine wave
									// send start condition
									i2c_start(slaveAddr);
									if (readInArr[2] == '0') {
										// command byte
										i2c_write(0x00);			// set DAC0 output
									}
									else if (readInArr[2] == '1') {
										i2c_write(0x01);			// set DAC1 output
									}
									
									// output byte
									
									i2c_write(sinWave[i]);
									
									_delay_ms((1/frequency)/64 * (800 - (frequency-10) * 18.56));	
									//_delay_ms(0.48);
									i2c_stop();
									//1/20/64*x = 0.48 614.4
									//1/10/64*x = 1.25 800
			
									

									
								}
								//USART_Transmit(readInArr[2]);
							}
														
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


	
	

	
