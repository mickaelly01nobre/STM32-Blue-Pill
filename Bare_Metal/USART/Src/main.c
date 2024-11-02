#include <stdint.h>
#include "stm32f1xx.h"
#include <stdlib.h>  // Para usar a função atoi()

#define CPU_CLK 	8000000
#define BaudRate	115200

char rx_buffer[3];  // Buffer para armazenar três caracteres
char* rx_ptr = rx_buffer;  // Ponteiro para o buffer
uint8_t MAIN_SM = 0;
uint8_t toggle_mode = 0;  // Variável para controlar o estado do toggle (0 = desligado, 1 = ligado)
uint8_t current_color = 0;  // Variável para controlar a cor atual no modo toggle

// Função para converter dois caracteres em um número inteiro
int convert_intensity(char tens, char units) {
    char intensity_str[3] = {tens, units, '\0'};  // Cria uma string a partir dos dois caracteres
    return atoi(intensity_str);  // Converte a string em um número inteiro
}

void USART1_IRQHandler(void)
{
    /* Verifica se há um dado recebido */
    if ((USART1->SR & USART_SR_RXNE))
    {
        /* Lê o dado recebido */
        char received_char = USART1->DR;

        /* Ignora caracteres de nova linha ou retorno de carro */
        if (received_char == '\n' || received_char == '\r')
            return;

        /* Armazena o caractere no buffer se houver espaço */
        if (rx_ptr < (rx_buffer + 3))
        {
            *rx_ptr = received_char;
            rx_ptr++;
        }

        /* Se o buffer estiver cheio (três caracteres), processa o comando */
        if (rx_ptr >= (rx_buffer + 3))
        {
            rx_ptr = rx_buffer;  // Reinicia o ponteiro
            MAIN_SM = 1;         // Sinaliza que o comando está completo
        }
    }
}

void toggle_colors(void) {
    switch (current_color) {
        case 0:  // Vermelho
            GPIOB->ODR |= 0x00000040;  // Liga o LED vermelho (PB6)
            GPIOB->ODR &= ~(0x00000080 | 0x00000200);  // Desliga os outros
            current_color = 1;  // Muda para verde na próxima iteração
            break;
        case 1:  // Verde
            GPIOB->ODR |= 0x00000080;  // Liga o LED verde (PB7)
            GPIOB->ODR &= ~(0x00000040 | 0x00000200);  // Desliga os outros
            current_color = 2;  // Muda para azul na próxima iteração
            break;
        case 2:  // Azul
            GPIOB->ODR |= 0x00000200;  // Liga o LED azul (PB9)
            GPIOB->ODR &= ~(0x00000040 | 0x00000080);  // Desliga os outros
            current_color = 0;  // Muda para vermelho na próxima iteração
            break;
    }
}


int main(void)
{
	//enable clock access to GPIOA
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	//enable clock access to GPIOB
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;

	//Enable clock access to alternate function
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;

	// Rx
	//Configure PA10(RX) as input floating
	GPIOA->CRH &= 0xFFFFF0FF;
	GPIOA->CRH |= 0x00000400;

	/**************************************************************************************
	 *
	 * 										Rx
	 *
	 **************************************************************************************/
	//enable clock access to USART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//Enable receiver
	USART1->CR1 |= USART_CR1_RE;

	//Set Baud Rate
	USART1->BRR = ((CPU_CLK + (BaudRate/2U))/BaudRate);

	//Enable RXNIE interrupt
	USART1->CR1|=USART_CR1_RXNEIE;
	//Enable USART1 interrupt in NVIC
	NVIC_EnableIRQ(USART1_IRQn);

	//Enable UART
	USART1->CR1 |= USART_CR1_UE;

	/**************************************************************************************
	 *
	 * 										PWM
	 *
	 **************************************************************************************/

	AFIO->MAPR&=~AFIO_MAPR_TIM4_REMAP;
	// Habilitar o clock para GPIOB e TIM4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;   // Habilita o clock para TIM4
	// Configura PB6, PB7, PB9 como saída alternativa push-pull para os LEDs (TIM4 channels)
	GPIOB->CRL &= 0x00FFFFFF;
	GPIOB->CRL |= 0x22000000;
	GPIOB->CRH &= 0xFFFFFF00;
	GPIOB->CRH |= 0x00000022;

	GPIOB->ODR &= 0x00000000;
	// Configurar TIM4 para gerar PWM
	TIM4->PSC = 799;   // Define o prescaler como 0 (sem divisão, frequência máxima)
	TIM4->ARR = 99;  // Define o valor máximo do contador para 100 (para PWM de 0 a 100%)

	// Configurar os canais 1, 2 e 4 do TIM4 para modo PWM
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;  // Configura PB6 (TIM4_CH1) para PWM mode 1
	TIM4->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;  // Configura PB7 (TIM4_CH2) para PWM mode 1
	TIM4->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;  // Configura PB9 (TIM4_CH4) para PWM mode 1

	// Habilitar os canais de saída
	TIM4->CCER |= TIM_CCER_CC1E;  // Habilita o canal 1 (PB6)
	TIM4->CCER |= TIM_CCER_CC2E;  // Habilita o canal 2 (PB7)
	TIM4->CCER |= TIM_CCER_CC4E;  // Habilita o canal 4 (PB9)

	// Iniciar o contador do TIM4
	TIM4->CR1 |= TIM_CR1_CEN;  // Habilita o contador do TIM4

    while(1)
    {
        if(MAIN_SM)
        {
            MAIN_SM = 0;  // Reseta o estado da máquina

            // Verifica se todos os três caracteres foram recebidos corretamente
            char tens = rx_buffer[1];   // Segundo caractere é a dezena
            char units = rx_buffer[2];  // Terceiro caractere é a unidade

            // Caso de 'D' ou 'd' para alternância de cores
            switch (rx_buffer[0])
            {
                case 'D':
                case 'd':
                    // Alterna o estado de toggle_mode
                    toggle_mode = !toggle_mode;  // Liga ou desliga o modo de toggle
                    continue;  // Volta para o início do loop

                case 'R':
                case 'r':
                    TIM4->CCR1 = convert_intensity(tens, units);
                    if (TIM4->CCR1 > 0) {
                        GPIOB->ODR |= 0x00000040;  // Liga o LED vermelho (PB6)
                    } else {
                        GPIOB->ODR &= ~0x00000040;  // Desliga o LED vermelho (PB6)
                    }
                    break;

                case 'G':
                case 'g':
                    TIM4->CCR2 = convert_intensity(tens, units);
                    if (TIM4->CCR2 > 0) {
                        GPIOB->ODR |= 0x00000080;  // Liga o LED verde (PB7)
                    } else {
                        GPIOB->ODR &= ~0x00000080;  // Desliga o LED verde (PB7)
                    }
                    break;

                case 'B':
                case 'b':
                    TIM4->CCR4 = convert_intensity(tens, units);
                    if (TIM4->CCR4 > 0) {
                        GPIOB->ODR |= 0x00000200;  // Liga o LED azul (PB9)
                    } else {
                        GPIOB->ODR &= ~0x00000200;  // Desliga o LED azul (PB9)
                    }
                    break;

                default:
                    // Caso o caractere de cor seja inválido, não faz nada
                    break;
            }
        }

        // Se o modo de toggle estiver ativado, alterna as cores
        if (toggle_mode)
        {
            toggle_colors();  // Chama a função que alterna as cores
            for (volatile int i = 0; i < 1000000; i++);  // Pequeno atraso para ver o efeito
        }
    }
}
