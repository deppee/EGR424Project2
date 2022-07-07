#include "msp.h"
/*
 * systick.c
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */


/**
 *  SysTick_init: Initialization of SysTick Timer to 3MHz, no interrupts. This
 *              portion of code is inspired by Emily Deppe's
 *              EGR226 slot machine project.
 *  INPUTS:
 *          none
 *  OUTPUTS:
 *          none
 */
void SysTick_Init(void) {
    SysTick -> CTRL = 0;            // disable SysTick During step
    SysTick-> LOAD = 0x00FFFFFF;    // max reload value
    SysTick -> VAL = 0;             // any write to current clears it
    SysTick -> CTRL = 0x00000005;   // enable systic, 3MHz, No Interrupts
}

/**
 *  SysTick_delay_ms: Uses SysTick Timer to delay for a given number of ms. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 *
 *  INPUTS:
 *      delay (unit16_t) length of delay in ms.
 *  OUTPUTS: none
 */
void SysTick_delay_ms (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3000) -1);          // delay for 1 msecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}

/**
 *  SysTick_delay_us: Uses SysTick Timer to delay for a given number of us. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 *  INPUTS:
 *      delay (unit16_t) delay in us
 *  OUTPUTS:
 *          none
 */
void SysTick_delay_us (uint16_t delay) {
    SysTick -> LOAD = ((delay * 3) -1);             // delay for 1 usecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}
