//
// RoboSlam pre-programmed example program for Dublin Maker 2015
// Code is for MSP430G2553
//
// Select between four modes by grounding P1.3, P1.4, P1.5 or none.
//
// Mode 0 (none grounded): zigzag
// Mode 1 (P1.3 grounded): stop motors, flash LED, print A0 via UART
// Mode 2 (P1.4 grounded): prowling, preprogrammed movement sequence
// Mode 3 (P1.5 grounded): IR sensor guidance, spin when A0 > 200
//
// Written by Ted Burke - last updated 25-7-2015
//

#include <msp430.h>
#include <stdio.h>

// These three bits refresent the state of pins P1.3-5
// i.e. have they been pulled down or not?
#define MODE_BITS ((P1IN >> 3) & 0b111)
#define MODE_0 0b111
#define MODE_1 0b110
#define MODE_2 0b101
#define MODE_3 0b011

// Function prototypes
void setup();
void motors(int left_speed, int right_speed);
void move(int left, int right, int time, int led_state);
void led(int state);
int read_analog_channel(unsigned int);

int main( void )
{
    // Configure pins, timers, etc.
    setup();

    int v; // analog voltage
    int n; // just a counter

    // Start-up LED sequence - 2 seconds of rapid blinking
    for (n=0 ; n<20 ; ++n)
    {
        led(1); __delay_cycles(50000);
        led(0); __delay_cycles(50000);
    }

    // Main loop
    while(1)
    {
        // Implement selected mode depending on which pins are grounded
        if (MODE_BITS == MODE_0)
        {
            // Mode 0: No pins grounded - default behaviour is zigzag
            if (MODE_BITS == MODE_0) move ( 100, 100,1000,1); // forward for 1000ms with LED on
            if (MODE_BITS == MODE_0) move (-100, 100,1000,0); // spin left for 1000ms with LED off
            if (MODE_BITS == MODE_0) move ( 100, 100,1000,1); // forward for 1000ms with LED on
            if (MODE_BITS == MODE_0) move ( 100,-100,1000,0); // spin right for 1000ms with LED off
        }
        else if (MODE_BITS == MODE_1)
        {
            // Mode 1: P1.3 is grounded - stop, blink LED and print A0 via UART

            // flash LED then read A0 and print value
            if (MODE_BITS == MODE_1) move (0,0,250,1); // stop for 250ms with LED on
            if (MODE_BITS == MODE_1) move (0,0,250,0); // stop for 250ms with LED off
            printf("Mode: P1.3, v = %04d\n", read_analog_channel(0));
        }
        else if (MODE_BITS == MODE_2)
        {
            // Mode 2: P1.4 is grounded - prowl around, preprogrammed sequence
            n = 3;
            while(n-- && (MODE_BITS == MODE_2))
            {
                move( 100,   0, 750, 1); // swerve right
                move(   0, 100, 750, 0); // swerve left
            }
            if (MODE_BITS == MODE_2) move(-100, 100,3000, 0); // spin left
            if (MODE_BITS == MODE_2) move( 100, 100,2000, 1); // forward
            if (MODE_BITS == MODE_2) move(-100,-100, 750, 0); // reverse
            if (MODE_BITS == MODE_2) move(-100,   0, 750, 0); // swerve back

        }
        else if (MODE_BITS == MODE_3)
        {
            // Mode 3: P1.5 is grounded - spin when A0 > 200
            v = read_analog_channel(0);
            printf("Mode: P1.5, v = %04d\n", v);
            if (v < 512) {motors(100,-100); led(0);} // spin on black
            else         {motors(100, 100); led(1);} // forward on high
        }
    }

    return 0;
}

void setup()
{
    // Watchdog timer
    WDTCTL = WDTPW + WDTHOLD; // Disable watchdog timer

    // Digital i/o
    P2OUT = 0b00000000; // set P2.0-7 low
    P2SEL = 0b00100100; // set pin 10 as TA1.1, pin 13 as TA1.2
    P2DIR = 0b00111111; // set P2.0-5 as outputs
    P1OUT = 0b00111000;
    P1REN = 0b00111000; // pull-up resistors on P1.3, P1.4, P1.5
    P1DIR = 0b00000010; // P1.1 is an output

    // configure Timer_A1 for motor PWM
    TA1CTL = TASSEL_2 + ID_0 + MC_1; // Timer_A1: SMCLK clock, input divider=1, "up" mode
    TA1CCR0 = 1000;                  // set Timer_A1 period to 1ms for 1kHz PWM
    TA1CCR1 = 1000;                  // 100% duty cycle initially
    TA1CCR2 = 1000;                  // 100% duty cycle initially
    TA1CCTL1 = OUTMOD_7;             // select "Reset/Set" output mode
    TA1CCTL2 = OUTMOD_7;             // select "Reset/Set" output mode

    // Analog inputs
    ADC10AE0 = 0b00000001; // A0 (pin 2) is an analog input
    ADC10CTL0 = ADC10ON;   // Turn on the ADC

    // Basic Clock Module (set to 1MHz)
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    // UART
    // Baudrate = 1MHz / (256 * UCA0BR1 + UCA0BR0)
    UCA0BR1 = 0; UCA0BR0 = 104;  // Set baudrate = 9600
    UCA0CTL1 |= UCSSEL_2;        // Set USCI clock to SMCLK
    UCA0MCTL = UCBRS0;           // Modulation UCBRSx = 1
    P1SEL = BIT2; P1SEL2 = BIT2; // Set P1.2 as TXD
    UCA0CTL1 &= ~UCSWRST;        // Start USCI (release from reset)
}

//
// For the printf function (from stdio.h) to work, we need to provide
// a putchar function which transmits a single character via the UART.
//
int putchar(int c)
{
    UCA0TXBUF = c;
    while((IFG2 & UCA0TXIFG) == 0);
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

//
// N.B. This function is not really being used as intended because
// I made a mistake with my wiring layout for the SN754410NE and
// speed control cannot currently be used.
//
// This function would set the speed and direction of both motors, the
// first argument controllings the left motor, the second controlling
//  the right motor. Each argument would be any integer between
// -100 and +100, where -100 is full speed reverse. +100 is full speed
// forwards.
//
// With the current wiring:
//   P2.1 and P2.3 control left motor
//   P2.0 and P2.4 control right motor
//
void motors(int left, int right)
{
    // work out P2OUT value for requested directions
    int p2out_value = 0;
    if (left > 0)  p2out_value += 0b00001000;
    if (left < 0)  p2out_value += 0b00000010;
    if (right > 0) p2out_value += 0b00010000;
    if (right < 0) p2out_value += 0b00000001;

    // rectify and clamp motor speed to allowed range
    if (left < 0)    left = -left;
    if (right < 0)   right = -right;
    if (left > 100)  left = 100;
    if (right > 100) right = 100;

    // set motor directions and speeds
    P2OUT = p2out_value;

    // Just set speed to full because wiring is wrong ;-(
    TA1CCR1 = 1000;
    TA1CCR2 = 1000;
}

//
// Sets the motors and LED state for the specified number
// of milliseconds (approximate). This is just a convenience
// function for programming sequences of movements.
//
void move(int left, int right, int time, int led_state)
{
    led(led_state);
    motors(left, right);
    while(time--) __delay_cycles(1000);
}

// Switch the LED on or off
void led(int state)
{
    if (state > 0) P1OUT |= BIT1; // LED on
    else P1OUT &= ~BIT1;          // LED off
}