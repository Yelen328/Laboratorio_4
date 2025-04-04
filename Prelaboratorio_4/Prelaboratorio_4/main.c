/*
 * Prelaboratorio_4.c
 *
 * Created: 4/4/2025 
 * Author : yelena cotzojay
 */ 
#define		F_CPU	16000000
#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

void setup()
{
	DDRB=0xFF;
	PORTB=0x00;
	DDRC&=~((1<<PB2)|(1<<PB3));
	PORTC&=~((1<<PB2)|(1<<PB3));
	PORTC|=(1<<PB2)|(1<<PB3);	
}

//INTERRUPCIONES 
