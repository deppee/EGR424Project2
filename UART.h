/*
 * UART.h
 *
 *  Created on: Jul 6, 2022
 *      Author: leigh
 */

#ifndef UART_H_
#define UART_H_

int UART_MSend(char *a);
int UART_Send(char a);
int UART_Receive(void);
void UART_init(void);


#endif /* UART_H_ */
