/*
 * LCD.h
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */

#ifndef LCD_H_
#define LCD_H_

#define RS 1 //P4.0
#define RW 2 //P4.1
#define EN 4 //P4.2


void WelcomeScreen(void);
void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_nibble_write(unsigned char data, unsigned char control);
void PlayScreen(void);
void userWon(void);
void userLost(void);
void checkWins(void);
void CreateCustomCharacter (unsigned char *Pattern, const char Location);

#endif /* LCD_H_ */
