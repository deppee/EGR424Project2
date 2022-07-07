/*
 * systick.h
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "msp.h"

void SysTick_Init(void);
void SysTick_delay_ms (uint16_t delay);
void SysTick_delay_us (uint16_t delay);

#endif /* SYSTICK_H_ */
