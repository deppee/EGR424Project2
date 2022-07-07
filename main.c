#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include "custom_characters.h"
#include "sound.h"
#include "systick.h"
#include "UART.h"
#include "LCD.h"
#include <string.h>

 /* LCD: VSS = Pmeter GND, VDD = Pmeter 5v, V0 = Pmeter Contrast
  *           RS = P4.0, RW = P4.1, E = P4.2, D4 = P4.4, D5 = P4.5, D6 = P4.6, D7 = P4.7, A = 5v, K = GND
  */

void Port1_init(void);
void PORT1_IRQHandler(void);

/* Global Variables */
// 0->idle, 1->first scroll, 2->check for win
int play = 0;
int n1[3] = {0,1,2};
int n2[3] = {3,4,5};
int n3[3] = {6,7,0};
int received = 0;

/**
 * main.c
 */
void main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    SysTick_Init();             // initialize SysTick Timer
    LCD_init();                 // initialize LCD
    Port1_init();               // initialize buttons
    Sound_init();

    NVIC_EnableIRQ(PORT1_IRQn); // enable interrupts on port 1
    __enable_interrupt();


    CreateCustomCharacter(smile,0);
    CreateCustomCharacter(music_note,1);
    CreateCustomCharacter(bell,2);
    CreateCustomCharacter(firework,3);
    CreateCustomCharacter(heart,4);
    CreateCustomCharacter(bomb,5);
    CreateCustomCharacter(ant,6);
    CreateCustomCharacter(man,7);
    CreateCustomCharacter(snowflake,8);

    WelcomeScreen();
    UART_init();
    UART_MSend("Let's play =]\n");
    SysTick_delay_ms(1000);

    while(1) {
        if(received == 83){
            received = 0;
            UART_MSend("Spinning!\n");
            play = 1;       // break out of loop and start the reel
        }
        if(received == 80){
            received = 0;
            play = 3;       // break out of loop and stop the reel
        }

        if (play == 1 || play == 2) {
            PlayScreen();           // play one scroll
        } else if (play == 3) {
            // check if user has won
            checkWins();
            play = 0;
        }
    }
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

