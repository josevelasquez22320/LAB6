/*
 * Prelab6.c
 *
 * Created: 23/04/2024 08:43:22
 * Author : josea
 */ 

#define  F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void initUART9600(void);
void writeUART(char caracter);
void writeTEXTUART(char* texto);
volatile char bufferRX;


int main(void)
{
	initUART9600();
	sei();
	/*writeUART("H")
	writeUART("O")
	writeUART("L")
	writeUART("A")
	writeUART(" ")
	writeUART("M")
	writeUART("U")
	writeUART("N")
	writeUART("D")
	writeUART("O")
	writeUART("/n")*/

	writeTEXTUART("HOLA MUNDO XD!\n");

	DDRB = 0xFF;
	PORTB = 0; 

    while (1)
    {
		PORTB = bufferRX;
    }
}

void initUART9600(void){
	//configurara tx y rx
	DDRD &= ~(1<< DDD0);
	DDRD |= (1<< DDD1);
	
	//CONFIG REGISTRO A
	UCSR0A =0;
	UCSR0A |= (1<<U2X0);
	
	//CONFIGURAR REGISTO B HABILITAR ISR RX Y TX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0) | (1<< RXEN0) | (1<< TXEN0);
	// CONFIGURAR REGISTRO C >FRAME: 8BITS DATOA, NOPARIDAD 1 DE STOP
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	//VELICIDAD BAUDRATE= 9600
	UBRR0 =207;
	
}

//MIESTRAS NO SEA CIERTO () HAZ LO QUE ESTE ADENTRO HASTA QUE SE CUMPLA
void writeUART(char caracter){
	while(!(UCSR0A & (1<<UDRE0)));
	//ESPERAR A QUE UDR0 ESTE EN 1
	UDR0 = caracter;
	
}

void writeTEXTUART(char* texto){
	uint8_t i;
	for(i = 0; texto[i]!='\0' ;i++){
		while(!(UCSR0A & (1<<UDRE0)));
		//ESPERAR A QUE UDR0 ESTE EN 1
		UDR0 = texto[i];	
	}
}



ISR(USART_RX_vect) {
	bufferRX = UDR0;
	
	
	while(!(UCSR0A & (1<<UDRE0)));
		//ESPERAR A QUE UDR0 ESTE EN 1
	UDR0 = bufferRX;
}