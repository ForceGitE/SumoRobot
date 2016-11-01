//
// RoboSlam example: ZigZag Motor Control
// Written 14-11-2015
// Code is for MSP430G2452 or MSP430G2553
//

#include <msp430.h>

// These functions make it simpler to control the LED
// and the motors and to read from the colour sensor
void led(int light);

void main()
{
    // stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // P2.0 and P2.1 control right motor
    // P2.4 and P2.5 control left motor
    // P1.0 controls the green LED
    P1DIR = 0b00000011;   // Port 1 outputs: P1.0 green LED, P1.7 red LED
    P2DIR = 0b00011011;
	int i = 0;
	int j = 0;

	for(i=0; i<=3;i++)
	{
		led(1);                  // Turn LED off
		__delay_cycles(1000000); // 1 second delay
		led(0);
		__delay_cycles(1000000); // 1 second delay
	}
	for(j=0; j<=10;j++)
	{
		led(1);                  // Turn LED off
		__delay_cycles(100000); // 1/2 second delay
		led(0);					//Turn Led on
		__delay_cycles(100000); // 1/2 second delay
	}

    led(1);                // Turn LED off
    while(1)
    {
	  P2OUT = 0b00000011;      // both motors forwards, LED off
      __delay_cycles(2000000); // 2 second delay
	}
}

// This function controls the motors.
// For each motor, 1=forward, 0=stop and -1=reverse.
//

// This function controls the LED.
//
// led(1) switches the LED on.
// led(0) switches the LED off.
//
void led(int on_off)
{
    if (on_off == 1) P1OUT |= 0b00000011;  // LED on
    if (on_off == 0) P1OUT &= ~0b00000011; // LED off
}

