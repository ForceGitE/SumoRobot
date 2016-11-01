//
// RoboSlam Example: Rangefinder test
// Code is for MSP430G2452 or MSP430G2553
// Written by Ted Burke - Last updated 11-3-2015
//

#include <msp430.h>

// main function
int main( void )
{
    // disable watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // configure digital inputs and outputs
    P1DIR = 0b00001000;   // Port 1 outputs: P1.3 (trig)
    P2DIR = 0b00111000;   // Port 2 outputs: P2.3 (LED), P2.4-5 (motor)

    // test LED and motors
    int d;
    while(1)
    {
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
        int d = 0;
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
            P2OUT = 0b00101000; // forward
        }
        else
        {
            P2OUT = 0b00010000; // reverse
        }

        __delay_cycles(100000);
    }
}