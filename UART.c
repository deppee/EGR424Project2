/*
 * UART.c
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */
#include "msp.h"
#include "string.h"

extern int received;

/*
 * UART_MSend: Send an entire string of data to UART.
 *
 * INPUTS:
 *      a (char*) string to send
 *
 * OUTPUTS:
 *      len (unsigned int) length of string that was sent
 *
 * NOTES:
 *
 */
int UART_MSend(char *a){
    unsigned int len = strlen(a);

    while(*a != '\0')
    {
        while(!(EUSCI_A0 -> IFG & 0x02));
        EUSCI_A0 -> TXBUF = *a++;
    }

    return len;
}

/*
 * EUSCIA0_IRQHandler:
 *
 * INPUTS:
 *      received (int) global variable that contains the single letter that was
 *                      received from BT.
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 */
void EUSCIA0_IRQHandler(void){
    if (EUSCI_A0->IFG & BIT0)  // Interrupt on the receive line
    {
        received = EUSCI_A0->RXBUF;
        received = (int)received;
        EUSCI_A0->IFG &= ~BIT0; // Clear the interrupt flag right away in case new data is ready
    }

}

/*
 * UART_int: Initialize UART capabilities at 115200 bps, no parity, 8-bits, 1-stop bit
 *           Enable interrupts
 *
 * INPUTS:
 *      none
 *
 * OUTPUTS:
 *      none
 *
 * NOTES:
 *      none
 */
void UART_init(void){

    P1->SEL0 |=  (BIT2 | BIT3); // P1.2 = RX, P1.3 = TX
    P1->SEL1 &= ~(BIT2 | BIT3);

    EUSCI_A0->CTLW0  |= BIT0;  // put UART software in reset mode
    EUSCI_A0->MCTLW  &= ~BIT0;  // disable oversampling mode
    EUSCI_A0->BRW = 26;          // set the baud rate as 3MHz / 115200
    EUSCI_A0->CTLW0  &= ~(BIT(15)|BIT(12)|BIT(11));   //BIT15 = no parity, BIT12 = 8-bit, BIT11 = 1-stop bit
    EUSCI_A0->CTLW0  |= (BIT7|BIT6);            // SMCLOCK
    EUSCI_A0->CTLW0  &= ~BIT0;  // enable EUSCI
    //  stop bit, no parity, SMCLK clock source and 8-bit data size
    EUSCI_A0->IFG &= ~BIT0;    // Clear interrupt
    EUSCI_A0->IE |= BIT0;      // Enable interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);
}
