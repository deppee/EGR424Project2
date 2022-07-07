/*
 * LCD.c
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */

#include "LCD.h"
#include "msp.h"
#include "UART.h"
#include "systick.h"
#include "sound.h"


extern int play;
extern int n1[3];
extern int n2[3];
extern int n3[3];
/*
 * CreateCustomCharacter: store a 8x8 bit character in CGRAM
 *      A maximum of 8 characters can be stored.
 *      Some code from: https://openlabpro.com/guide/custom-character-lcd-pic/
 *
 * INPUTS:
 *      Pattern (Unsigned char*) the bit array of the character
 *      Location (const char) where in CGRAM to store the character
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 *
 */

void CreateCustomCharacter (unsigned char *Pattern, const char Location)
{
    int i=0;
    LCD_command(0x40+(Location*8));     //Send the Address of CGRAM
    for (i=0; i<8; i++)
        LCD_data(Pattern [ i ] );         //Pass the bytes of pattern on LCD
}
/*
 * Checkwins: check if the same character is present 3 in a row in any of the rows
 *
 * INPUTS:
 *      n1, n2, n3 (int) global variables.  Contains the address of the chracter in CGRAM
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 *
 */
void checkWins(void) {

    if ((n1[0] == n1[1]) && (n1[0] == n1[2]) ||     // horizontal win
        (n2[0] == n2[1]) && (n2[0] == n2[2]) ||
        (n3[0] == n3[1]) && (n3[0] == n3[2])) {
        userWon();
    } else {
        userLost();
    }
}

/*
 * UserWon: if the user won then play the win sound and print a win message to BT and the LCD
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 *
 */
void userWon(void) {
    int i;
    LCD_command(1);                         // clear display
    SysTick_delay_us(100);
    char thirdLine[] = "  You've Won!!";       // prints to second line
    UART_MSend("Great job, you've won!\n");
    int thirdNum = 14;                      // number of characters on second line

    SysTick_delay_ms(500);

    LCD_command(0x90);                      // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum; i++) {
        LCD_data(thirdLine[i]);
        SysTick_delay_ms(1);
    }

    SysTick_delay_ms(1000);
    SoundTwo();
}

/*
 * Userlost: if the user lost then play the lose sound and print a lose message to BT and the LCD
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 *
 */

void userLost(void) {

    int i;
    LCD_command(1);                         // clear display
    SysTick_delay_us(100);
    char firstLine[] = "  You've Lost!";    // prints to first line
    int firstNum = 14;                      // number of characters on first line
    char thirdLine[] = "    Try Again";     // prints to second line
    UART_MSend("Sorry, you lost! Try again?\n");
    int thirdNum = 13;                      // number of characters on second line

    SysTick_delay_ms(500);

    LCD_command(0x80);                      // set cursor at center of first line
    SysTick_delay_us(100);
    for(i = 0; i < firstNum; i++){
        LCD_data(firstLine[i]);
        SysTick_delay_ms(1);
    }

    LCD_command(0x90);                       // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum; i++) {
        LCD_data(thirdLine[i]);
        SysTick_delay_ms(1);
    }
    SoundOne();
}

/*
 * PlayScreen: This function controls and displays the symbols
 *          that represent the slot machine. This portion
 *          of code is inspired by Emily Deppe's EGR226
 *          slot machine project.
 *
 * INPUTS:
 *      n1, n2, n3 (int) global variables.  Contains the address of the chracter in CGRAM
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 *
 */
void PlayScreen(void) {

    uint8_t i;

    // generate a new row
    srand(time());
    char c;
    n1[0] = rand() % 7;
    n1[1] = rand() % 7;
    n1[2] = rand() % 7;

    // clear LCD if first scroll
    if (play == 1) {
        LCD_command(1);
        play = 2;
    }

    SysTick_delay_ms(500);
    LCD_command(0x85);                   //set cursor at center of first line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(n3[i]);
        SysTick_delay_us(1);
    }

    LCD_command(0xC5);                   //set cursor at center of second line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(n2[i]);
        SysTick_delay_us(1);
    }

    LCD_command(0x95);                      //set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(n1[i]);
        SysTick_delay_us(1);
    }

    // shift rows up

    int temp[3];
    temp[0] = n2[0];
    temp[1] = n2[1];
    temp[2] = n2[2];

    n2[0] = n1[0];
    n2[1] = n1[1];
    n2[2] = n1[2];

    n3[0] = temp[0];
    n3[1] = temp[1];
    n3[2] = temp[2];

     SysTick_delay_ms(500);
}

/**
 * Welcome screen: Prints a Welcome message to the LCD screen. This
 *          portion of code is inspired by Emily Deppe's
 *          EGR226 slot machine project.
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *
 */
void WelcomeScreen(void) {

    int i;
    LCD_command(1);                             // clear display
    SysTick_delay_us(100);
    char firstLine[] = "Leigha & Emily's";      // prints to first line
    int firstNum = 16;                          // number of characters on first line
    char thirdLine[] = "  Slot Machine";        // prints to second line
    int thirdNum = 14;                          // number of characters on second line
    UART_MSend("Leigha and Emily's slot machine!");

    SysTick_delay_ms(500);

    LCD_command(0x80);                      // set cursor at center of first line
    SysTick_delay_us(100);
    for(i = 0; i < firstNum; i++){
        LCD_data(firstLine[i]);
        SysTick_delay_ms(1);
    }

    LCD_command(0x90);                       // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum; i++) {
        LCD_data(thirdLine[i]);
        SysTick_delay_ms(1);
    }


    SysTick_delay_ms(2000);
    LCD_command(1);                             // clear display
    SysTick_delay_us(100);
    char firstLine2[] =  "Let's play...";       // prints to first line
    int firstNum2 = 13;                         // number of characters on first line
    char thirdLine2[] = "SPIN TO WIN!";         // prints to third line
    UART_MSend("Spin to WIN!");
    int thirdNum2 = 12;                         // number of characters on third line
    SysTick_delay_ms(500);

    LCD_command(0x80);                          // set cursor at center of first line
    SysTick_delay_us(100);
    for(i = 0; i < firstNum2; i++) {
        LCD_data(firstLine2[i]);
        SysTick_delay_ms(1);
    }

    LCD_command(0x90);                          // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum2; i++) {
        LCD_data(thirdLine2[i]);
        SysTick_delay_ms(1);
    }

    SysTick_delay_ms(3000);
}

/**
 * LCD_init: Initialization function to set up pins for LCD screen. This
 *          portion of code is inspired by Emily Deppe's
 *          EGR226 slot machine project.
 *
 * INPUTS: none
 *
 * OUTPUTS:
 *      none
 */
void LCD_init(void) {
    P4->DIR = 0xFF;                     // make P4 pins output for data and controls
    SysTick_delay_ms(30);               // begin initialization sequence
    LCD_nibble_write(0x30,0);
    SysTick_delay_ms(10);
    LCD_nibble_write(0x30,0);
    SysTick_delay_ms(1);
    LCD_nibble_write(0x30,0);
    SysTick_delay_ms(1);
    LCD_nibble_write(0x20,0);           // use 4 bit data mode
    SysTick_delay_ms(1);
    LCD_command(0x28);                  // set 4 bit data, 2 line, 5x7 font
    SysTick_delay_us(100);
    LCD_command(0x06);                  // move cursor right after each char
    SysTick_delay_us(100);
    LCD_command(0x01);                  // clear screen, move cursor to home
    SysTick_delay_us(100);
    LCD_command(0x0F);                  // turn on display, cursor blinking
}

/**
 *  LCD_command: Sends commands to the LCD screen for initialization. This
 *                  portion of code is inspired by Emily Deppe's
 *                  EGR226 slot machine project.
 *  INPUTS:
 *          command (unsigned char) the 8-bit command to send to the LCD
 * OUTPUTS:
 *      none
 */
void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    // upper nibble first
    LCD_nibble_write(command << 4, 0);      // lower nibble next
    if(command < 4)
        SysTick_delay_ms(4);
    else
        SysTick_delay_ms(1);
}

/**
 *  LCD_data: Sends data to the LCD screen. This
 *          portion of code is inspired by Emily Deppe's
 *          EGR226 slot machine project.
 *  INPUTS:
 *      data (unsigned char) the 8-bit data to send to the LCD
 *
 *  OUTPUTS:
 *          none
 */
void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);  // upper nibble
    LCD_nibble_write(data << 4, RS);    // lower nibble
    SysTick_delay_ms(1);
}

/**
 *  LCD_nibble_write: Controls the data that is sent to the LCD screen. This
 *              portion of code is inspired by Emily Deppe's
 *              EGR226 slot machine project.
 *  INPUTS:
 *          data (unsigned char)
 *          control (unsigned char)
 *
 *  OUTPUTS:
 *          none
 */
void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;                   // clear lower nibble for control
    control &= 0x0F;                // clear upper nibble for data
    P4->OUT = data | control;       // RS = 0, R/W = 0
    P4->OUT = data | control | EN;  // pulse E high
    SysTick_delay_us(100);
    P4->OUT = 0;                    // clear E
}
