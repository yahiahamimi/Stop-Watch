/******************************************************************************
 *
 * File Name: project2.c
 *
 * Description: source file for project2
 *
 * Author: Yahia Hamimi
 ******************************************************************************/

#include "project2.h"

/* global variables representing every fraction of time*/
unsigned char num_sec0 = 0;
unsigned char num_sec1 = 0;
unsigned char num_min0 = 0;
unsigned char num_min1 = 0;
unsigned char num_hr0 = 0;
unsigned char num_hr1 = 0;

/* Interrupt Service Routine for timer1 compare mode */
ISR(TIMER1_COMPA_vect)
{
	/* If time exceeds 99:59:59 start from the beginning */
	if (num_hr1 == 9 && num_hr0 == 9 && num_min1 == 5 && num_min0 == 9 && num_sec1 == 5 && num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1 = 0;
		num_min0 = 0;
		num_min1 = 0;
		num_hr0 = 0;
		num_hr1 = 0;
	}
	/* If time = 09:59:59 the next step is 10:00:00 */
	else if (num_hr0 == 9 && num_min1 == 5 && num_min0 == 9 && num_sec1 == 5 && num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1 = 0;
		num_min0 = 0;
		num_min1 = 0;
		num_hr0 = 0;
		num_hr1++;
	}
	/* If time = 00:59:59 the next step is 01:00:00 */
	else if (num_min1 == 5 && num_min0 == 9 && num_sec1 == 5 && num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1 = 0;
		num_min0 = 0;
		num_min1 = 0;
		num_hr0++;
	}
	/* If time = 00:09:59 the next step is 00:10:00 */
	else if (num_min0 == 9 && num_sec1 == 5 && num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1 = 0;
		num_min0 = 0;
		num_min1++;
	}
	/* If time = 00:00:59 the next step is 00:01:00 */
	else if (num_sec1 == 5 && num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1 = 0;
		num_min0++;
	}
	/* If time = 00:00:09 the next step is 00:00:10 */
	else if (num_sec0 == 9)
	{
		num_sec0 = 0;
		num_sec1++;
	}
	else
	{
		/* Increment the first 7-segment every interrupt(every second) */
		num_sec0++;
	}
}
/* Interrupt Service Routine for external Interrupt0 (reset the stop watch) */
ISR(INT0_vect)
{
	/* Reset every time fraction to 0 */
	num_sec0 = 0;
	num_sec1 = 0;
	num_min0 = 0;
	num_min1 = 0;
	num_hr0 = 0;
	num_hr1 = 0;
	/* Connect the clock bits again in case that we came from the stop state */
	TCCR1B |= (1 << CS10) | (1 << CS12);
}
/* Interrupt Service Routine for external Interrupt1 (pause the stop watch) */
ISR(INT1_vect)
{
	/* Disable the clock source (bits) */
	TCCR1B &= ~(1 << CS10) & ~(1 << CS12);
}
/* Interrupt Service Routine for external Interrupt2 (resume the stop watch) */
ISR(INT2_vect)
{
	/* Reconnect the clock source (bits) */
	TCCR1B |= (1 << CS10) | (1 << CS12);
}

/* Choose the biggest prescaler to get the smallest frequency to ensure the best Ttimer possible
 * with N=1024 Timer frequency will be around 1Khz, Ttimer = 1ms.
 * So we just need 1000 counts to get 1s period.
 * Make compare value = 1000 to get an interrupt every second.
 */
void Timer1_CTC_Init(void)
{
	TCNT1 = 0;				/* Set timer1 initial count to zero */
	OCR1A = 1000;			/* Set the Compare value to 1000 equal one second */
	TIMSK |= (1 << OCIE1A); /* Enable Timer1 Compare A Interrupt */

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=0
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1 << FOC1A);

	/* Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. Prescaler = F_CPU/1024 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
}
/* External INT0 enable and configuration function */
void External_Interrupt0_Init(void)
{
	MCUCR |= (1 << ISC01); /* Trigger INT0 with the falling edge */
	DDRD &= ~(1 << PD2);   /* Configure INT0/PD2 as input pin */
	PORTD |= (1 << PD2);   /* Activate internal pull-up resistor */
	GICR |= (1 << INT0);   /* Enable external interrupt pin INT0 */
}
/* External INT1 enable and configuration function */
void External_Interrupt1_Init(void)
{
	MCUCR |= (1 << ISC10) | (1 << ISC11); /* Trigger INT0 with the raising edge */
	DDRD &= ~(1 << PD3);				  /* Configure INT1/PD3 as input pin */
	GICR |= (1 << INT1);				  /* Enable external interrupt pin INT1 */
}
/* External INT2 enable and configuration function */
void External_Interrupt2_Init(void)
{
	MCUCSR &= ~(1 << ISC2); /* Trigger INT2 with the falling edge */
	DDRB &= ~(1 << PB2);	/* Configure INT2/PB2 as input pin */
	PORTB |= (1 << PB2);	/* Activate internal pull-up resistor */
	GICR |= (1 << INT2);	/* Enable external interrupt pin INT2 */
}
/*function to loop over the 7-seg to give each one the power for certain time
 * and adjust the corresponding bit to the 7-seg the result that all numbers seems to be here
 * at the same time due to the short delay(dosen't exceed 30ms so eye can't detect it)
 */
void Sevenseg_Looping_Display(void)
{
	PORTA = (PORTA & 0xC0) | (1 << PA0);
	PORTC = (PORTC & 0xF0) | (num_sec0 & 0x0F);
	_delay_ms(5);
	PORTA = (PORTA & 0xC0) | (1 << PA1);
	PORTC = (PORTC & 0xF0) | (num_sec1 & 0x0F);
	_delay_ms(5);
	PORTA = (PORTA & 0xC0) | (1 << PA2);
	PORTC = (PORTC & 0xF0) | (num_min0 & 0x0F);
	_delay_ms(5);
	PORTA = (PORTA & 0xC0) | (1 << PA3);
	PORTC = (PORTC & 0xF0) | (num_min1 & 0x0F);
	_delay_ms(5);
	PORTA = (PORTA & 0xC0) | (1 << PA4);
	PORTC = (PORTC & 0xF0) | (num_hr0 & 0x0F);
	_delay_ms(5);
	PORTA = (PORTA & 0xC0) | (1 << PA5);
	PORTC = (PORTC & 0xF0) | (num_hr1 & 0x0F);
	_delay_ms(5);
}

void IOPort_Init(void)
{
	DDRC |= 0x0F;	  /* Configure the first 4-pins in PORTC as output pins */
	DDRA |= 0x3F;	  /* Configure the first 6-pins in PORTA as output pins */
	PORTA &= 0xC0;	  /*set the pins initially to 0 */
	PORTC &= 0xF0;	  /*set the pins initially to 0 */
	SREG |= (1 << 7); /* Enable global interrupts in MC */
}

int main(void)
{
	IOPort_Init();
	Timer1_CTC_Init();
	External_Interrupt0_Init();
	External_Interrupt1_Init();
	External_Interrupt2_Init();

	while (1)
	{
		Sevenseg_Looping_Display();
	}
}
