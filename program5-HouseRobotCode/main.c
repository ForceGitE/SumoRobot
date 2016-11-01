//
// RoboSlam: House Robot Code for Seek & Destroy competition.
// Code is for MSP430G2553
//

#include <msp430.h>

int read_analog_channel(unsigned int);

int main( void )
{
	 int v; // analog voltage
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

     // Analog inputs
    ADC10AE0 = 0b00000001; // A0 (pin 2) is an analog input
    ADC10CTL0 = ADC10ON;   // Turn on the ADC

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
			 // P1OUT |= BIT1; //Turn on LED

		}
		else
		{
			TA1CCR1 = 400; //Slow down motors
			//P1OUT &= ~BIT1; //turn off LED
		}

		//Read IR Sensor
        v = read_analog_channel(0);
        if (v < 512)  // reverse on Black
        {
			P1OUT |= BIT1;
			P2SEL  = 0b00000000;
			P2OUT = 0b00001100;
			 __delay_cycles(2000000);
			 P2SEL  = 0b00000100;

    		P2OUT = 0b00000011;

        }
        else {P1OUT &= ~BIT1;}


        __delay_cycles(100000);


    }

    return 0;
}

//
// This function performs a single analog to digital conversion,
// converting the voltage on analog input pin ANx into a 10-bit
// unsigned integer. Execution time for this function is of the
// order of 100us.
//
int read_analog_channel(unsigned int x)
{
    ADC10CTL0 &= ~ENC;            // disable conversion
    ADC10CTL1 = x << 12;          // select channel
    ADC10CTL0 |= ENC;             // enable conversion
    ADC10CTL0 |= ADC10SC;         // start conversion
    while(ADC10CTL1 & ADC10BUSY); // wait until complete
    return ADC10MEM;              // return digital value
}