/*
 * sound.h
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */

#ifndef SOUND_H_
#define SOUND_H_

void Sound_init(void);
void PlaySound(int pitch, int duration,unsigned short NotePause);
void SoundOne(void);
void SoundTwo(void);

#endif /* SOUND_H_ */
