#include "msp.h"
/*
 * systick.c
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */


/**
 * Initialization of SysTick Timer. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void SysTick_Init(void) {
    SysTick -> CTRL = 0;            // disable SysTick During step
    SysTick-> LOAD = 0x00FFFFFF;    // max reload value
    SysTick -> VAL = 0;             // any write to current clears it
    SysTick -> CTRL = 0x00000005;   // enable systic, 3MHz, No Interrupts
}

/**
 * Uses SysTick Timer to delay for a given number of ms. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void SysTick_delay_ms (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3000) -1);          // delay for 1 msecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}

/**
 * Uses SysTick Timer to delay for a given number of us. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void SysTick_delay_us (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3) -1);             // delay for 1 usecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}
