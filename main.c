#include "msp.h"
#include <stdio.h>
#include <stdbool.h>

 /* LCD: VSS = Pmeter GND, VDD = Pmeter 5v, V0 = Pmeter Contrast
  *           RS = P4.0, RW = P4.1, E = P4.2, D4 = P4.4, D5 = P4.5, D6 = P4.6, D7 = P4.7, A = 5v, K = GND
  *
  * BUZZER: one end to GND, one end to P3.7
  */

#define RS 1 //P4.0
#define RW 2 //P4.1
#define EN 4 //P4.2

void WelcomeScreen(void);
void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_nibble_write(unsigned char data, unsigned char control);
void SysTick_Init(void);
void SysTick_delay_ms (uint16_t delay);
void SysTick_delay_us (uint16_t delay);
void PlayScreen(void);
void Port1_init(void);
void PORT1_IRQHandler(void);
void checkWins(void);
void userWon(void);
void userLost(void);
void Sound_init(void);
void PlaySound(int pitch, int duration,unsigned short NotePause);
void SoundOne(void);
void SoundTwo(void);

/* Global Variables */
// 0->idle, 1->first scroll, 2->check for win
int play = 0;
char row1[] = "@#^$?%*!";
char row2[] = "%!*?@^#";
char row3[] = "~$^&*@%=!#+";

/**
 * main.c
 */
void main(void) {

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	SysTick_Init();             // initialize SysTick Timer
	LCD_init();                 // initialize LCD
	Port1_init();               // initialize buttons
	Sound_init();               // initialize sound

	NVIC_EnableIRQ(PORT1_IRQn); // enable interrupts on port 1
	__enable_interrupt();

	WelcomeScreen();
	SysTick_delay_ms(1000);

	while(1) {
	    if (play == 1 || play == 2) {
	        PlayScreen();           // play one scroll
	    } else if (play == 3) {
	        // check if user has won
	        checkWins();
	        play = 0;
	    }
	}
}

void checkWins(void) {

    if ((row1[0] == row2[0]) && (row1[0] == row3[0]) ||
        (row1[1] == row2[1]) && (row1[1] == row3[1]) ||
        (row1[2] == row2[2]) && (row1[2] == row3[2])) {
        userWon();
    } else {
        userLost();
    }
}

void userWon(void) {
    int i;
    LCD_command(1);                         // clear display
    SysTick_delay_us(100);
    char thirdLine[] = "  You've Won!!";       // prints to second line
    int thirdNum = 14;                      // number of characters on second line

    SysTick_delay_ms(500);

    LCD_command(0x90);                      // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum; i++) {
        LCD_data(thirdLine[i]);
        SysTick_delay_ms(1);
    }

    SoundTwo();
}

void userLost(void) {

    int i;
    LCD_command(1);                         // clear display
    SysTick_delay_us(100);
    char firstLine[] = "  You've Lost!";    // prints to first line
    int firstNum = 14;                      // number of characters on first line
    char thirdLine[] = "    Try Again";     // prints to second line
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

/**
 * This function controls and displays the symbols
 *  the represent the slot machine. This portion
 *  of code is inspired by Emily Deppe's EGR226
 *  slot machine project.
 */
void PlayScreen(void) {

    uint8_t i;

    // shift rows
    row1[0] = row2[0];
    row1[1] = row2[1];
    row1[2] = row2[2];
    row1[3] = row2[3];
    row1[4] = row2[4];
    row1[5] = row2[5];
    row1[6] = row2[6];

    row2[0] = row3[0];
    row2[1] = row3[1];
    row2[2] = row3[2];
    row2[3] = row3[3];
    row2[4] = row3[4];
    row2[5] = row3[5];
    row2[6] = row3[6];

    // generate a new row
    srand(time());
    char c;
    int n = rand() % 10;
    for (i = 0; i < n; ++i) {
        c = row3[0];
        row3[0] = row3[1];
        row3[1] = row3[2];
        row3[2] = row3[3];
        row3[3] = row3[4];
        row3[4] = row3[5];
        row3[5] = row3[6];
        row3[6] = row3[7];
        row3[7] = row3[8];
        row3[8] = row3[9];
        row3[9] = row3[10];
        row3[10] = c;
     }

    // clear LCD if first scroll
    if (play == 1) {
        LCD_command(1);
        play = 2;
    }

    SysTick_delay_ms(500);
    LCD_command(0x85);                   //set cursor at center of first line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(row1[i]);
        SysTick_delay_us(1);
    }

    LCD_command(0xC5);                   //set cursor at center of second line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(row2[i]);
        SysTick_delay_us(1);
    }

    LCD_command(0x95);                      //set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < 3; i++) {
        LCD_data(row3[i]);
        SysTick_delay_us(1);
    }

     SysTick_delay_ms(500);
}

/**
 * Initialization function to set up ports P1.1 and P1.4
 *  as buttons with interrupts.
 */
void Port1_init(void) {
    P1->SEL0 &=~ (BIT1 | BIT4);
    P1->SEL1 &=~ (BIT1 | BIT4);
    P1->DIR &=~ (BIT1 | BIT4);
    P1->REN |= (BIT1 | BIT4);
    P1->OUT |= (BIT1 | BIT4);
    P1->IES |= (BIT1 | BIT4);
    P1->IE |= (BIT1 | BIT4);
    P1->IFG = 0;
}

/**
 * Controls the interrupts of ports P1.0 and P1.1
 *  as buttons with interrupts.
 */
void PORT1_IRQHandler(void) {

    // if button is pressed, play the game.
    if ((P1->IN & BIT1)== 0) {
        printf("playyy\n");
        play = 1;
        P1->IFG &= ~BIT1;
    }

    // if button is pressed, stop the game.
    if ((P1->IN & BIT4)== 0) {
        printf("stop\n");
        play = 3;
        P1->IFG &= ~BIT4;
    }
}

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



/**
 * Prints a Welcome message to the LCD screen. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void WelcomeScreen(void) {

    int i;
    LCD_command(1);                             // clear display
    SysTick_delay_us(100);
    char firstLine[] = "Leigha & Emily's";      // prints to first line
    int firstNum = 16;                          // number of characters on first line
    char thirdLine[] = "  Slot Machine";        // prints to second line
    int thirdNum = 14;                          // number of characters on second line

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

    SysTick_delay_ms(2000);
}

/**
 * Initialization function to set up pins for LCD screen. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
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
 * Sends commands to the LCD screen for initialization. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
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
 * Sends data to the LCD screen. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void LCD_data(unsigned char data) {
    LCD_nibble_write(data & 0xF0, RS);  // upper nibble
    LCD_nibble_write(data << 4, RS);    // lower nibble
    SysTick_delay_ms(1);
}

/**
 * Controls the data that is sent to the LCD screen. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void LCD_nibble_write(unsigned char data, unsigned char control) {
    data &= 0xF0;                   // clear lower nibble for control
    control &= 0x0F;                // clear upper nibble for data
    P4->OUT = data | control;       // RS = 0, R/W = 0
    P4->OUT = data | control | EN;  // pulse E high
    SysTick_delay_us(100);
    P4->OUT = 0;                    // clear E
}

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
void SysTick_delay_ms(uint16_t delay) {
    SysTick -> LOAD = ((delay * 3000) -1);          // delay for 1 msecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}

/**
 * Uses SysTick Timer to delay for a given number of us. This
 *  portion of code is inspired by Emily Deppe's
 *  EGR226 slot machine project.
 */
void SysTick_delay_us(uint16_t delay) {
    SysTick -> LOAD = ((delay * 3) -1);             // delay for 1 usecond per delay value
    SysTick -> VAL = 0;                             // any write to CVR clears it
    while ( (SysTick-> CTRL & 0x00010000) == 0);    // wait for flag to be SET
}





