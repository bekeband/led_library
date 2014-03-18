/* 
 * File:   LED.h
 * Author: bekeband
 *
 * Created on 2013. június 14., 12:23
 * Header file for LED library. This library help the ELD_187 type LED display module,
 * for LED control. Use the timer1 module for multiplexered LED outputs. The LED controls
 * have in timer1 interrupt routine.
 * WARNING !!! The LED library uses several resources. These resources of
 * - Init
 */

#ifndef led_h
#define	led_h

#ifdef	__cplusplus
extern "C" {
#endif

/* ELBAG 8873 ELD_187 LED display driver library
 * Type of LED
 */

enum e_LED_COLOR { GREEN, RED };
enum e_LED_STATE { OFF = 0, ON = 1, BLINK = 2};
enum e_LED_POS { L1 = 0, L2 = 1, L3 = 2, L4 = 3, T1 = 4, T2 = 5, T3 = 6, T4 = 7,
    CO = 8, ST = 9 };

/******************************************************************************/
/* struct LED_PORT structure */
/******************************************************************************/

typedef struct {
    volatile unsigned int* ADDRESS_GR;   // Portaddress, and bitnumber of green side
    int bitnumb_GR;
    volatile unsigned int* ADDRESS_RD;   // Portaddress, and bitnumber of red side
    int bitnumb_RD;
} LED_PORT;

/******************************************************************************/
/* struct LED_STATE contains LED_STATE, and color */
/******************************************************************************/

typedef struct {
    enum e_LED_STATE STATE;     // OFF, BLINK, ON
    enum e_LED_COLOR COLOR;     // RED, GREEN
} LED_STATE;

extern LED_STATE LED_STATES[10];

/* Input DIP switches mirror word. */

extern unsigned int INPUT_DIPS;

/******************************************************************************
 * PORTSET(volatile unsigned int* ADDRESS, int number) Set the number'th bit of
 * ADDRESS; Another bits not changed.
 ******************************************************************************/

void PORTSET(volatile unsigned int* ADDRESS, int number);

/******************************************************************************
 * PORTSET(volatile unsigned int* ADDRESS, int number) Reset the number'th bit of
 * ADDRESS; Another bits not changed.
 ******************************************************************************/

void PORTRES(volatile unsigned int* ADDRESS, int number);

/* ON the number'th LED with setting the output lines and datas.
 * void LED_ON(enum e_LED_COLOR color, int number) when
 * e_LED_COLOR : enum of led color (change of the polarity)
 * number : LED number (0-9)
 */

void LED_ON(enum e_LED_COLOR color, int number); /* set number'th LED ON */
void LED_OFF(enum e_LED_COLOR color, int number); /* set number'th LED OFF */
//void SetAllLEDforInput();   /* Set ALL LED lines for input.   */
void Init_LED_Display();    /* Initialize LED display timer, and timer interrupt.*/

#ifdef	__cplusplus
}
#endif

#endif	/* led_h */

