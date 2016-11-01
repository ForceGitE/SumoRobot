//
// RoboSlam Example: PWM Example
// Code is for MSP430G2452 or MSP430G2553
//

#include <msp430.h>

int main( void )
{
    // Watchdog timer
    WDTCTL = WDTPW + WDTHOLD; // Disable watchdog timer

    P1DIR = 0b00001010;   // Port 1 outputs: P1.3 (trig), LED on port P1.1
    P1OUT = 0b00000000;


    // Select primary peripheral module function on P2.2 (i.e. TA1.1)
    P2SEL  = 0b00000100;
    P2SEL2 = 0b00000000;

    P2DIR  = 0b00011111; // Pin must also be configured as an output
    P2OUT = 0b00000011;

    // Configure Timer A interrupt
    TA1CTL = TASSEL_2 + ID_0 + MC_1; // Timer_A1: SMCLK clock, input divider=1, "up" mode
    TA1CCR0 = 1000;                  // Set Timer_A1 period to 1ms for 1kHz PWM
    TA1CCR1 = 400;                   // 50% duty cycle initially (500us pulse width)
    TA1CCTL1 = OUTMOD_7;             // Select "Reset/Set" output mode

    // This loop is just an example of varying the PWM duty cycle on
    // TA1.1 simply by updating the value of TA1CCR1.

    int d;
    while(1)
    {


         // Send 20us trigger pulse
		P1OUT |= BIT3; //P1OUT = P1OUT || BIT3
		__delay_cycles(20);
		P1OUT &= ~BIT3;

		// Wait for start of echo pulse
		while((P1IN & BIT2) == 0)
		{
			//Do nothing

		}

		// Measure how long the pulse is

		 d = 0;
		while((P1IN & BIT2) > 0)
		{
			// The following delay was worked out by trial and error
			// so that d counts up in steps corresponding to 1cm
			__delay_cycles(30);
			d = d + 1;

			if (d >= 400) break;
		}

		if (d < 30)
		{

			  TA1CCR1 = 1000; //Speed up motors
			  P1OUT |= BIT1; //Turn on LED

		}
		else
		{
			TA1CCR1 = 400; //Slow down motors
			P1OUT &= ~BIT1; //turn off LED
		}

        __delay_cycles(100000);


    }

    return 0;
}