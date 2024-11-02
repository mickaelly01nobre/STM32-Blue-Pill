#ifndef UART_H_
#define UART_H_

#include "stdio.h"

void uart2_init();
void uart_receive_time(int *hours, int *minutes, int *seconds);
int uart_receive_number();


#endif /* UART_H_ */
