/*
 * Prelaboratorio_4.c
 *
 * Created: 4/4/2025 
 * Author : yelena cotzojay
 */ 
#define	F_CPU	16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t	contador=0; //Contador de núero entero de 8 bits
volatile uint8_t	estado=0X00;


//Función Prototypes
void setup();
void INIT_PIN_CHANGE();

//main
int main(void)
{
    setup();
	while (1) {
		PORTB = contador;  // Actualiza PORTB con el valor del contador
	}
	
}

void setup()
{
	cli();
	DDRB =0xFF;		//Configurarlos como saldia
	PORTB =0xff;		//Inicialmente apagado
	DDRC &=~((1<<PC2)|(1<<PC3));	//Configurar el bit 2 y 3 del puerto C como entrada
	PORTC|=(1<<PC2)|(1<<PC3);		//Pull up activado
	INIT_PIN_CHANGE();
	sei();
}

void INIT_PIN_CHANGE()
{//Habilitar interrupciones de pin change 
	PCICR |= (1 << PCIE1);     // Habilitar interrupción de cambio de estado 
	PCMSK1 |= (1 << PCINT10);  // Habilita interrupción para PC2 (bit 2 de Puerto C)
	PCMSK1 |= (1 << PCINT11);  // Habilita interrupción para PC3 (bit 3 de Puerto C)
}

//Vector de interupción:
ISR(PCINT1_vect) {
	//Leer estado actua de los botones
	uint8_t	estado_actual=PINC;
	
	//Detectar flanco
	if ((estado_actual & (1<<PC2)) &&(estado&(1<<PC2)))
	{
		contador++;
	}
	if ((estado_actual & (1<<PC23)) &&(estado&(1<<PC3)))
	{
		contador--;
	}
	} 