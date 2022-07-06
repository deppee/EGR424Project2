#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include "custom_characters.h"
#include <string.h>

 /* LCD: VSS = Pmeter GND, VDD = Pmeter 5v, V0 = Pmeter Contrast
  *           RS = P4.0, RW = P4.1, E = P4.2, D4 = P4.4, D5 = P4.5, D6 = P4.6, D7 = P4.7, A = 5v, K = GND
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

void CreateCustomCharacter (unsigned char *Pattern, const char Location);

void setupSerial_BT();
void UART_init(void);
void writeOutput(char *string);
void readInput(char *string);

#define BUFFER_SIZE 100
char INPUT_BUFFER[BUFFER_SIZE];
char string[BUFFER_SIZE];
uint8_t read_location = 0; // used in the main application to read valid data that hasn't been read yet

/* Global Variables */
// 0->idle, 1->first scroll, 2->check for win
int play = 0;
int n1[3] = {0,0,0};
int n2[3] = {0,0,0};
int n3[3] = {0,0,0};
uint8_t storage_location = 0; // used in the interrupt to store new data

/**
 * main.c
 */
void main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    SysTick_Init();             // initialize SysTick Timer
    LCD_init();                 // initialize LCD
    Port1_init();               // initialize buttons

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
    setupSerial_BT();
    writeOutput("Initialization successful!");
    SysTick_delay_ms(1000);

    while(1) {
        readInput(string);                  // Read the input up to \n, store in string.  This function doesn't return until \n is received
        printf("%s\n", string);

        if (play == 1 || play == 2) {
            PlayScreen();           // play one scroll
        } else if (play == 3) {
            // check if user has won
            checkWins();
            play = 0;
        }
    }
}


void CreateCustomCharacter (unsigned char *Pattern, const char Location)
{
    int i=0;
    LCD_command(0x40+(Location*8));     //Send the Address of CGRAM
    for (i=0; i<8; i++)
        LCD_data(Pattern [ i ] );         //Pass the bytes of pattern on LCD
}

void checkWins(void) {

    if ((n1[0] == n1[1]) && (n1[0] == n1[2]) ||
        (n2[0] == n2[1]) && (n2[0] == n2[2]) ||
        (n3[0] == n3[1]) && (n3[0] == n3[2])) {
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
    writeOutput("Great job, you've won!");
    int thirdNum = 14;                      // number of characters on second line

    SysTick_delay_ms(500);

    LCD_command(0x90);                      // set cursor center of third line
    SysTick_delay_us(100);
    for(i = 0; i < thirdNum; i++) {
        LCD_data(thirdLine[i]);
        SysTick_delay_ms(1);
    }

    SysTick_delay_ms(1000);
}

void userLost(void) {

    int i;
    LCD_command(1);                         // clear display
    SysTick_delay_us(100);
    char firstLine[] = "  You've Lost!";    // prints to first line
    int firstNum = 14;                      // number of characters on first line
    char thirdLine[] = "    Try Again";     // prints to second line
    writeOutput("Sorry, you lost!");
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
}

/**
 * This function controls and displays the symbols
 *  the represent the slot machine. This portion
 *  of code is inspired by Emily Deppe's EGR226
 *  slot machine project.
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
    writeOutput("Leigha and Emily's slot machine!");

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
    writeOutput("Spin to WIN!");
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

void setupSerial_BT()
{

    P1->SEL0 |=  (BIT2 | BIT3); // P1.2 and P1.3 are EUSCI_A0 RX
    P1->SEL1 &= ~(BIT2 | BIT3); // and TX respectively.

    EUSCI_A0->CTLW0  = BIT0; // Disables EUSCI. Default configuration is 8N1
    EUSCI_A0->CTLW0 |= BIT7; // Connects to SMCLK BIT[7:6] = 10
    EUSCI_A0->CTLW0 |= (BIT(15)|BIT(14)|BIT(11));  //BIT15 = Parity, BIT14 = Even, BIT11 = Two Stop Bits
    EUSCI_A0->BRW = 1;  // UCBR Value from above
    EUSCI_A0->MCTLW = 0x01A1; //UCBRS (Bits 15-8) & UCBRF (Bits 7-4) & UCOS16 (Bit 0)

    EUSCI_A0->CTLW0 &= ~BIT0;  // Enable EUSCI
    EUSCI_A0->IFG &= ~BIT0;    // Clear interrupt
    EUSCI_A0->IE |= BIT0;      // Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);


}

void EUSCIA0_IRQHandler(void)   // UART CodeBlocks
{
    if (EUSCI_A0->IFG & BIT0)  // Interrupt on the receive line
    {
        INPUT_BUFFER[storage_location] = EUSCI_A0->RXBUF; // store the new piece of data at the present location in the buffer
        EUSCI_A0->IFG &= ~BIT0; // Clear the interrupt flag right away in case new data is ready
        storage_location++; // update to the next position in the buffer
        if(storage_location == BUFFER_SIZE) // if the end of the buffer was reached, loop back to the start
            storage_location = 0;
    }
}

void readInput(char *string)
{
    int i = 0;  // Location in the char array "string" that is being written to
    char temp[BUFFER_SIZE];
    // One of the few do/while loops I've written, but need to read a character before checking to see if a \n has been read
    do
    {
        // If a new line hasn't been found yet, but we are caught up to what has been received, wait here for new data
        while(read_location == storage_location && INPUT_BUFFER[read_location] != '\n');
        temp[i] = INPUT_BUFFER[read_location];  // Manual copy of valid character into "string"
        INPUT_BUFFER[read_location] = '\0';
        i++; // Increment the location in "string" for next piece of data
        read_location++; // Increment location in INPUT_BUFFER that has been read
        if(read_location == BUFFER_SIZE)  // If the end of INPUT_BUFFER has been reached, loop back to 0
            read_location = 0;
    }
    while(temp[i-1] != '\n'); // If a \n was just read, break out of the while loop

    temp[i-1] = '\0'; // Replace the \n with a \0 to end the string when returning this function
        temp[i-2] = '\0';   // just for bluetooth, the app ends statements with LF + CR instead of \n

    strcpy(string, temp);
}


void writeOutput(char *string)
{

    int i = 0;  // Location in the char array "string" that is being written to
    // send to CodeBlocks UART
    while(string[i] != '\0') {
        EUSCI_A0->TXBUF = string[i];
        i++;
        while(!(EUSCI_A0->IFG & BIT1));
    }


//    // send a null after every write to break out of read loop
//    if(string[0] == '/0')
//    {
//    //send null
//        EUSCI_A0->TXBUF = '\0';
//        while(!(EUSCI_A0->IFG & BIT1));
//    }
//
//    while(UCA0STATW & UCBUSY);

//    int tx_index = 0;
//    int length = (strlen(data)+1);
//
//    while(tx_index <= (strlen(data)+1)){
//
//        // Wait for TX buffer to be ready for new data
//
//        while(!(UCA0IFG & UCTXIFG));
//
//
//
//        // Push data to TX buffer
//
//        UCA0TXBUF = data[tx_index];
//
//
//
//        // Increment index
//
//        tx_index++;
//
//    }
//
//
//
//    // Wait until the last byte is completely sent
//
//    while(UCA0STATW & UCBUSY);
}


