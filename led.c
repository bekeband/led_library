
#include "led.h"
#include <p30Fxxxx.h>        /* Device header file                            */

/* Array of LED ports. These array contains the LED portaddress
 * (must be declared the TRIS register), and bitnumbers.
 */

LED_PORT LED_PORTS[10]= {{&TRISE, 0x04, &TRISE, 0x05},{&TRISE, 0x05, &TRISE, 0x08},
                         {&TRISE, 0x08, &TRISC, 0x0D},{&TRISE, 0x05, &TRISC, 0x0D},
                         {&TRISE, 0x04, &TRISE, 0x08},{&TRISE, 0x04, &TRISC, 0x0D},
                         {&TRISC, 0x0D, &TRISC, 0x0E},{&TRISE, 0x08, &TRISC, 0x0E},
                         {&TRISE, 0x05, &TRISC, 0x0E},{&TRISE, 0x04, &TRISC, 0x0E}};

/*State of display LED's */
int DISPLAY_STATE = 0;

/* Counter, and flag of display LED blinking... */
int DISPLAY_COUNTER = 0;
int HALF_SEC_FLAG = 0;

LED_STATE LED_STATES[10] = {{ON, GREEN},{ON, RED},{ON, GREEN},{ON, GREEN},
 {ON, RED},{ON, RED},{ON, GREEN},{ON, RED},{BLINK, GREEN},{ON, RED}};

/* void InitTimer1() */

void InitTimer1()
{
    T1CON = 0;  // Clear timer 1 control register
    TMR1 = 0;   // clear content the tmr register.
    PR1 = 0x09C4;   // PR1 period register for 0,001 secundum.
//    PR1 = 0xFFFF;    // Load the period register

/*  TCKPS<1:0>: Timer Input Clock Prescale Select bits
 *  11 = 1:256 prescale value
 *  10 = 1:64 prescale value
 *  01 = 1:8 prescale value
 *  00 = 1:1 prescale value */

    T1CONbits.TCKPS = 0b01;     // 1:64 prescaler

    /* TCS: Timer Clock Source Select bit
     * 1 = External clock from pin TxCK
     * 0 = Internal clock (FOSC/4)*/

    T1CONbits.TCS = 0;

    IPC0bits.T1IP = 0b100;  // T1 interrupt assign the level 1
    IFS0bits.T1IF = 0;      // Clear the TMR1 interrupt status flag.
    IEC0bits.T1IE = 1;      // Enable TIMER1 interrupt.
    T1CONbits.TON = 1;      // Start TMR1 timer.
}

void SetAllLEDforInput()
{
  TRISE |= 0b0000000100110000; // 4,5,8 input
  TRISC |= 0b0110000000000000; // 13, 14 input
}

/* Init_LED_Display Initialize LED display timer, and timer interrupt.
 * 
 */

void Init_LED_Display()   
{
    InitTimer1();
}

/******************************************************************************
 * PORTSET(volatile unsigned int* ADDRESS, int number) Set the number'th bit of
 * ADDRESS; Another bits not changed.
 ******************************************************************************/

void PORTSET(volatile unsigned int* ADDRESS, int number)
{   int ordata;
    ordata = 1 << number;
    *ADDRESS |= ordata;
}

/******************************************************************************
 * PORTSET(volatile unsigned int* ADDRESS, int number) Reset the number'th bit of
 * ADDRESS; Another bits not changed.
 ******************************************************************************/

void PORTRES(volatile unsigned int* ADDRESS, int number)
{   int anddata = 0;
    anddata = ~(1 << number);
    *ADDRESS &= anddata;
}


/* ON the number'th LED with setting the output lines and datas.
 * void LED_ON(enum e_LED_COLOR color, int number) when
 * e_LED_COLOR : enum of led color (change of the polarity)
 * number : LED number (0-9)
 */

void LED_ON(enum e_LED_COLOR color, int number)
{
    SetAllLEDforInput();
    // Set the desired PORT bits to output (TRIS register reset)
    PORTRES(LED_PORTS[number].ADDRESS_GR, LED_PORTS[number].bitnumb_GR);
    PORTRES(LED_PORTS[number].ADDRESS_RD, LED_PORTS[number].bitnumb_RD);
    asm("nop");
    // Output lines in indexed way.
    if (color == GREEN) {
        PORTSET((LED_PORTS[number].ADDRESS_GR)+0x02, LED_PORTS[number].bitnumb_GR);
        // TRISX + 0x02 (int) = LATX
        PORTRES((LED_PORTS[number].ADDRESS_RD)+0x02, LED_PORTS[number].bitnumb_RD);
        }
    else {
        PORTRES((LED_PORTS[number].ADDRESS_GR)+0x02, LED_PORTS[number].bitnumb_GR);
        // TRISX + 0x02 (int) = LATX
        PORTSET((LED_PORTS[number].ADDRESS_RD)+0x02, LED_PORTS[number].bitnumb_RD);
    }
}

void LED_OFF(enum e_LED_COLOR color, int number)
{
    SetAllLEDforInput();
    PORTRES(LED_PORTS[number].ADDRESS_GR, LED_PORTS[number].bitnumb_GR);
    PORTRES(LED_PORTS[number].ADDRESS_RD, LED_PORTS[number].bitnumb_RD);
    asm("nop");
    // Reset both LED output ports.
    PORTRES((LED_PORTS[number].ADDRESS_GR)+0x02, LED_PORTS[number].bitnumb_GR);
    PORTRES((LED_PORTS[number].ADDRESS_RD)+0x02, LED_PORTS[number].bitnumb_RD);
}

/******************************************************************************/
/* _T1Interrupt every 0,001 sec                                               */
/******************************************************************************/

void __attribute__((interrupt,auto_psv)) _T1Interrupt(void)
{
    if (DISPLAY_COUNTER++ == 500)
    {
        HALF_SEC_FLAG = 1;
    } else
        if (DISPLAY_COUNTER++ >= 1000)
        {
            HALF_SEC_FLAG = 0;
/*            U1TXREG = 'A';
            if (TXCHAR == 80) TXCHAR = ' ';*/
            DISPLAY_COUNTER = 0;
        };

//        GetInputBits();

        if ((DISPLAY_STATE) == 10)
        {
//            GetInputBits();
            DISPLAY_STATE = 0;
        }
        if (LED_STATES[DISPLAY_STATE].STATE > OFF)
        {
        if ((LED_STATES[DISPLAY_STATE].STATE == ON) ||
                    ((LED_STATES[DISPLAY_STATE].STATE == BLINK) && HALF_SEC_FLAG))
            {
                LED_ON(LED_STATES[DISPLAY_STATE].COLOR, DISPLAY_STATE);
            } else
                LED_OFF(LED_STATES[DISPLAY_STATE].COLOR, DISPLAY_STATE);
        } else
        {
            LED_OFF(LED_STATES[DISPLAY_STATE].COLOR, DISPLAY_STATE);
        }
        DISPLAY_STATE++;
//    }
    // clear the interrupt flag
    IFS0bits.T1IF = 0;  // reset TMR1 interrupt flag.
}
