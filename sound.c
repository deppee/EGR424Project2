/*
 * sound.c
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */
#include "systick.h"
#include "msp.h"

void Sound_init(void) {
    P3->DIR |= BIT7;
    P3->OUT &= ~BIT7;
}

void PlaySound(int pitch, int duration,unsigned short NotePause) {
    uint32_t i=0;
    for(i = 0; i < duration; i++){
        P3->OUT |= BIT7;
        SysTick_delay_ms(pitch);
        P3->OUT &= ~BIT7;
        SysTick_delay_ms(pitch);
     }
    SysTick_delay_ms(NotePause);
}


void SoundOne(void) {
    PlaySound(2, 200, 100);
    PlaySound(4, 100, 100);
    PlaySound(6, 150, 100);
}

void SoundTwo(void){
    PlaySound(2, 200, 100);
    PlaySound(2, 50, 50);
    PlaySound(2, 50, 50);
    PlaySound(2, 50, 50);
    PlaySound(2, 100, 50);
    PlaySound(2, 100, 100);
    PlaySound(2, 400, 100);

}
