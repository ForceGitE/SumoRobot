/*
 ============================================================================
 Name        : robot test code
 Author      : Paul Killeen
 Student No. : B00084482
 Created on  : 17/02/15
 Description : playtime code
 ============================================================================
 */

#include <msp430.h>

void setup();
void motors(char direction);
void led(int light);
int seek();

int main()
{
	char dir;

	setup();

	while(1)
	{
		if(P1IN & BIT0 == 1)
		{
			led(1);
			dir = 's';
			motors(dir);
			__delay_cycles(100000); //half-second delay

			dir = 'r';
			motors(dir);
			__delay_cycles(2000000);
		}
		else
		{
			led(0);
			dir = 'f';
			motors(dir);
		}
	}

	return 0;
}

void setup()
{
	// stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

	// P2.0 and P2.4 control left motor
    // P2.1 and P2.3 control right motor
	
    // P1.0 IR sensor
	// P1.1 LED
	// P1.2 echo
	// P1.3 trigger
	
    P1DIR = 0b00001010;
    P2DIR = 0b00011011;
    P1OUT = 0b00000000;


}

void led(int on_off)
{
    if (on_off == 1) P1OUT |= 0b00000010;  // LED on
    if (on_off == 0) P1OUT &= ~0b00000010; // LED off
}

void motors(char direction)
{

	switch(direction)
	{
		case 'f':		//forward
			P2OUT = 0b00000011;
			break;
		case 'b':		//back
			P2OUT = 0b00011000;
			break;
		case 'l':		//left
			P2OUT = 0b00010010;
			break;
		case 'r':		//right
			P2OUT = 0b00001001;
			break;
		default:		//stop
			P2OUT = 0b00000000;
	}
}


//ultrasonic sensor code
//returns a cemtimeter value
//which is the distance of
//object in front
int seek()
{
	int d = 0; //distance value
	
	// Send 20us trigger pulse
	P1OUT |= BIT3;
	__delay_cycles(20);
	P1OUT &= ~BIT3;

	// Wait for start of echo pulse
	while((P1IN & BIT2) == 0)
	{
		// nothing here
	}

	// Measure how long the pulse is
	while((P1IN & BIT2) > 0)
	{
		// The following delay was worked out by trial and error
		// so that d counts up in steps corresponding to 1cm
		__delay_cycles(30);
		d = d + 1;
		if (d >= 400) break;
	}
	return d;
}