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
/*
 * PlaySound: play a note on the piezo buzzer
 *
 * INPUTS:
 *      pitch (int) length of delay
 *      duration (int) length of note
 *      NotePause (unsigned short) delay between notes
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *
 */
// play a note on the piezo buzzer
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

/*
 * SoundOne: lose sound
 *
 * INPUTS: none
 *
 * OUTPUTS: none
 *
 * NOTES:
 *
 */
void SoundOne(void) {
    PlaySound(2, 200, 100);
    PlaySound(4, 100, 100);
    PlaySound(6, 150, 100);
}
/*
 * SoundTwo: win sound
 *
 * INPUTS: none
 *
 * OUTPUTS: none
 *
 * NOTES:
 *
 */
void SoundTwo(void){
    PlaySound(2, 200, 100);
    PlaySound(2, 50, 50);
    PlaySound(2, 50, 50);
    PlaySound(2, 50, 50);
    PlaySound(2, 100, 50);
    PlaySound(2, 100, 100);
    PlaySound(2, 400, 100);

}
