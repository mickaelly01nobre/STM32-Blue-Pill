

#include "uart.h"
#include "stm32f1xx.h"

#define CPU_CLK 	8000000
#define BaudRate	115200

char ch = '\0';


void uart2_write(int channel)
{

	while(!(USART2->SR & USART_SR_TXE)){}


	USART2->DR	=  (channel & 0xFF);
}


int __io_putchar(int channel)
{
	 uart2_write(channel);
	 return channel;

}

void uart2_init()
{



	/*UART2*/

	//habilita o clock do GPIOA
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	//Habilita o acesso para funcao alternativa
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;

	//Configura PA2 como output para o Tx
	GPIOA->CRL|=GPIO_CRL_MODE2;
	GPIOA->CRL|=GPIO_CRL_CNF2_1;
	GPIOA->CRL&=~GPIO_CRL_CNF2_0;

	// Configura PA10 como input para o Rx


	/*Configuracao da USART2*/

	//Habilita o clock da USART2 do Tx
	RCC->APB1ENR|=RCC_APB1ENR_USART2EN;
	// Habilita o clock da USART1 para o Rx
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;


	//Habilita a transmissao
	USART2->CR1 |= USART_CR1_TE;
	// Habilita a recepcao
	USART1->CR1 |= USART_CR1_RE;

	/*Confugura baud rate */
	USART2->BRR = ((CPU_CLK + (BaudRate/2U))/BaudRate); // Tx
	USART1->BRR = ((CPU_CLK + (BaudRate/2U))/BaudRate); // Rx

	//Habilita a USART
	USART2->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_UE;




}
