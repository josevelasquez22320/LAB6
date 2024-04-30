/*
;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
;Lab2.asm
;AUTOR: Jose Andrés Velásquez Gacía
;PROYECTO: postlab6
;HARDWARE: ATMEGA328P
;CREADO: 30/04/2024
;ÚLTIMA MODIFICACIÓN: 30/04/2024 23:36*/
  

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>


void initUART9600(void);
void initADC(void);
void mostrarTexto(char* text);
void desplegar(void);

volatile char bufferRX;		//volatil para cualquier cambio
uint8_t funcion = 0;
uint8_t conversion = 0;
uint8_t envioDato = 0;



int main(void)
{
	cli();
	DDRD = 0xC0;		//PD6 y PD7 as output
	DDRB = 0xFF;		//PB0 - PB7 as output
	DDRC &= ~(1 << PINC0);	//PC0 ENTRADA
	
	initUART9600();
	initADC();
	sei();
//habilitar interrupciones desplegar desplegar
	desplegar();
	
    while (1) 
    {
		if (funcion == 1){
			ADCSRA |= (1 << ADSC);
			PORTB = ADCH;
			PORTD = 0xC0 & ADCH;
		}else{
			funcion = 0;
		}
    }
}


//fun


void desplegar(void){
	mostrarTexto("\n\n\n   OPCIONES\n");
	mostrarTexto("1. Leer potenciómetro\n");
	mostrarTexto("2. Enviar ASCII\n");
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
	//ESPERAR A QUE UDR0 ESTE EN 1

void mostrarTexto(char* text){
	for (uint8_t i = 0; text[i] != '\0'; i++)
	{
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = text[i];
	}
}


//ISR, recieve
ISR(USART_RX_vect){
	bufferRX = UDR0;
	
	//if buffer is emptym, if it is not, it waits
	while(!(UCSR0A & (1 << UDRE0)));
	char copiaB = bufferRX;
	
	conversion = atoi(&copiaB);//con esa funcion numero lo convierte en entero no en caracter
	
	if (envioDato == 0){
		//op 1 ver que hacemos 
		switch (conversion){
			case 1:
				funcion = 1;
				UDR0 = bufferRX;
				mostrarTexto("\n\n");
				desplegar();
				break;
			//caso 2 esperara 2 turnos
			case 2:
				funcion = 0;
				envioDato = 1;
				UDR0 = bufferRX;
				mostrarTexto("\nPor favor seleccioone un caracter");
				mostrarTexto("\n\n");
				PORTD = 0;
				PORTB = 0;
				break;
			
			default:
			funcion = 0;
		}
		
	}else {
		envioDato = 0;
		UDR0 = bufferRX;
		PORTB = bufferRX;
		PORTD = 0xC0 & bufferRX;
		desplegar();
	}
}


// ADC

void initADC(void){
	ADMUX = 0;
	//5Vs
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	ADMUX |= (1 << ADLAR);	
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	
	//convertir ADC
	ADCSRA |= (1 << ADIE);	//INTERRUPCION
	
	//prescaler 128 > 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	DIDR0 |= (1 << ADC0D);	//PC0 COMO ENTRADA
}

ISR (ADC_vect){
	ADCSRA |= (1 << ADIF);	
}
