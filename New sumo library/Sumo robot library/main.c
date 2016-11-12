#include <msp430.h>

#define P1DIR 0b00001010   // Port 1 outputs: P1.3 (trig), LED on port P1.1
#define P1DIR 0b00001010   // Port 1 outputs: P1.3 (trig), LED on port P1.1
#define P1OUT 0b00000000
// Select primary peripheral module function on P2.2 (i.e. TA1.1)
#define P2SEL 0b00000100
#define P2SEL2 0b00000000
#define P2DIR  0b00011111 // Pin must also be configured as an output
#define P2OUT 0b00000011

int read_analog_channel(unsigned int);
void config timer();
void watchDog_Off();

// Configure Timer A interrupt
void config timer()
{
    TA1CTL = TASSEL_2 + ID_0 + MC_1; // Timer_A1: SMCLK clock, input divider=1, "up" mode
    TA1CCR0 = 1000;                  // Set Timer_A1 period to 1ms for 1kHz PWM
    TA1CCR1 = 400;                   // 50% duty cycle initially (500us pulse width)
    TA1CCTL1 = OUTMOD_7;             // Select "Reset/Set" output mode
}

void watchDog_Off()
{
    //does not allow overflow
    // Watchdog timer
    WDTCTL = WDTPW + WDTHOLD; // Disable watchdog timer
}
