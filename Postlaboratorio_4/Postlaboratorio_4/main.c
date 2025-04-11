/*UNIVERSIDAD DEL VALLE DE GUATEMALA
 * Postlaboratorio_4.c
 *
 * Created: 6/4/2025 16:29:50
 * Author : yelena Cotzojay
 */ 

#define	F_CPU	16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t	contador=0; //Contador de núero entero de 8 bits
volatile uint8_t	estado=0X00;
uint8_t valor_ADC=0;

uint8_t Pbaja=0;
uint8_t P_alta=0;
uint8_t display1=0;
uint8_t display2=0;
uint8_t MULTIPLEX=0;
uint8_t DISPLAY[] = {0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7F, 0x39, 0x5E, 0x79, 0x71};


//Función Prototypes
void	setup();
void	INIT_PIN_CHANGE();
void	INIT_TMR0();
void	INIT_ADC();
//main
int main(void)
{
	setup();
	
	while (1) {
	}
}

void setup()
{
	cli();			//Apagar interrupciones
	//Configuraciones de prescaler principal
	CLKPR= (1<<CLKPCE);
	CLKPR =(1<<CLKPS2);	//Setear el prescaler en 16 -> 1MHz
	
	
	INIT_TMR0();	//Llamar a la configuración del timer
	DDRB =0xFF;		//Configurarlos como saldia
	PORTB =0x00;		//Inicialmente apagado
	
	DDRD =0xFF;		//Configurarlos como saldia
	PORTD =0x00;		//Inicialmente apagado
	
	DDRC &= ~((1 << DDC2) | (1 << DDC3));	//Configurar el bit 2 y 3 del puerto C como entrada
	PORTC|=(1<<PORTC2)|(1<<PORTC3);		//Pull up activado
	
	DDRC |=((1 << DDC0) | (1 << DDC1)| (1 << DDC4) | (1 << DDC5));	//Configurar el bit 0 y 1 del puerto C como salida
	PORTC&=~((1<<PORTC4)|(1<<PORTC5));		//Inicialmente apagado
	UCSR0B = 0x00; // Inicialmente apagado
	INIT_PIN_CHANGE();
	INIT_ADC();
	ADCSRA |= (1 << ADSC);
	
	sei();				//Encender interrupciones
}

//****************SUBRUTINAS**************//

void INIT_PIN_CHANGE()
{//Habilitar interrupciones de pin change
	PCICR |= (1 << PCIE1);     // Habilitar interrupción de cambio de estado
	PCMSK1 |= (1 << PCINT10);  // Habilita interrupción para PORTC2 (bit 2 de Puerto C)
	PCMSK1 |= (1 << PCINT11);  // Habilita interrupción para PORTC3 (bit 3 de Puerto C)
}

void INIT_TMR0()
{
	TCCR0A =0;	//Configurarlo en modo normal
	TCCR0B |= (1<< CS01)  ;	//configurar un prescaler de 8
	TCNT0 =50;	//Desborde cada 1 ms
	TIMSK0 = (1 << TOIE0);	//Habilitar interrupciones
}

void	INIT_ADC()
{
	ADMUX = 0;	//Apagar todo
	
	//Voltaje de referencia 5V
	ADMUX |=(1<<REFS0);
	ADMUX &=~(1<<REFS1);
	
	ADMUX |= (1<<ADLAR); //orientación (izquierda)
	ADMUX |= (1<<MUX2) | (1<<MUX1);//Selección de canal Bit 6 del puerto C
	
	ADCSRA = 0;	//Apagar todo
	ADCSRA |=(1<<ADPS1)	| (1 << ADPS0); //Configuración del presacaler 8
	ADCSRA |= (1 << ADIE);	//Habilitación de interrupciones
	ADCSRA |= (1 << ADEN);	//Habilitar ADS
}

//**********Vector de interupción:************************/
ISR(PCINT1_vect) {
	//Leer estado actua de los botones
	uint8_t	estado_actual=PINC;
	
	//Detectar flanco
	if (!(estado_actual & (1<<PINC2))){
		contador++;
	}
	else if (!(estado_actual & (1<<PINC3))){
		contador--;
	}
	PORTB = contador;  // Actualiza PORTB con el valor del contador
	if (contador & (1 << 6))
	{
		PORTC |= (1 << PORTC0);
	}else{
		PORTC &= ~(1 << PORTC0);
	}
	if (contador & (1 << 7))
	{
		PORTC |= (1 << PORTC1);
	}else{
		PORTC &= ~(1 << PORTC1);
	}
}

ISR(ADC_vect)
{
	uint8_t temporal = ADCH;
	Pbaja= temporal & 0x0F;
	P_alta= (temporal>>4) & 0x0F;
	display2 =DISPLAY[Pbaja];
	display1 =DISPLAY[P_alta];
	
	if (temporal > contador)
	{
		PORTD |= (1 << PORTD7);
	}else{
		PORTD &= ~(1 << PORTD7);
	}

	ADCSRA |= (1 << ADSC);
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 =50;	//Desborde cada 5 ms
	PORTC &= ~((1<<PORTC4)|(1<<PORTC5));	//apagar todos los displays
	MULTIPLEX++;			//incrementar multiplex
	MULTIPLEX = MULTIPLEX % 2;
	PORTD = (PORTD & 0b10000000);
	switch (MULTIPLEX){
		case 0:
			//display1 = DISPLAY[4];
			PORTD = PORTD | display1;
			PORTC |=(1 << PORTC4);
			break;
		case 1:
			//display2 = DISPLAY[3];
			PORTD = PORTD | display2;
			PORTC |=(1 << PORTC5);
			break;		
		default:
			break;
	}
}
