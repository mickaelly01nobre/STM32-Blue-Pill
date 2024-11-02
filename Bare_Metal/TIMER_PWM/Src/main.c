/**
 ******************************************************************************
									PWM
 ******************************************************************************
 * Registradores TIMx_CNT e TIMx_CCRx :
 * 	-	TIMx_CNT: É o registro que contém o valor atual do contador do temporizador.
 * 	-	TIMx_CCRx: É o registro que contém o valor de comparação para o PWM.
 * 	Como funciona:
 * 	-	O temporizador compara o valor atual do contador (TIMx_CNT) com o
 * 		valor de comparação (TIMx_CCRx) para decidir se o sinal PWM deve estar "ligado" ou "desligado".
 * 		Assim:
 *
 * 		1.Se o valor de TIMx_CCRx é menor ou igual ao valor de TIMx_CNT (TIMx_CCRx ≤ TIMx_CNT), o temporizador
 * 		considera que o sinal deve estar em um estado específico (por exemplo, ligado).
 *
 * 		2. o valor de TIMx_CCRx é maior ou igual ao valor de TIMx_CNT (TIMx_CNT ≤ TIMx_CCRx), o sinal pode
 * 		estar em outro estado (por exemplo, desligado).
 *
 *	OBSERVACAO: O comportamento da comparação pode depender da direção do contador, que pode estar contando
 *				para cima ou para baixo.O comportamento da comparação pode depender da direção do contador,
 *				que pode estar contando para cima ou para baixo.
 *
 *
 * ----------------------------------------------------------------------------------------------------------------
 * ETRF e OCREF
 *
 *	1. ETRF (External Trigger):
 *		É um sinal externo que pode influenciar o comportamento do PWM. Pode ser usado para forçar mudanças
 *		no sinal PWM de acordo com eventos externos.
 *
 *	2.OCREF:
 *	-	É o sinal de referência de comparação do PWM que funciona da seguinte forma:O sinal OCREF pode ser limpo
 *		(ou redefinido) por um evento externo através do sinal ETR até o próximo período PWM. Isso garante que o
 *		sinal PWM possa ser ajustado em resposta a eventos externos.
 *
 *	-	O sinal OCREF é afirmado (ou ativado) quando há uma mudança no resultado da comparação entre TIMx_CNT e TIMx_CCRx.
 *		Isso pode indicar uma mudança no estado do PWM, como uma transição entre "ligado" e "desligado".
 *
 *	-	O sinal OCREF também é afirmado quando o modo de comparação de saída muda. Por exemplo: Se os bits OCxM no registro
 *		TIMx_CCMRx estavam configurados para a configuração "congelada" (sem comparação, OCxM=‘000), e agora mudam para
 *		um dos modos PWM (OCxM=‘110 ou ‘111), o sinal OCREF será afirmado.
 *
 *	OBSERVACAO: Configuração "Congelada": Isso significa que o temporizador não está realizando comparações e, portanto,
 *				não está gerando o PWM de acordo com o valor de TIMx_CCRx.
 ************************************************************************************************************************
 */

#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include "stm32f1xx.h"

#define BLINK_INTERVAL_MS 10

volatile uint8_t timerInterruptFlag = 0;
volatile uint8_t increasing = 1;  // Flag para indicar se o duty cycle está aumentando ou diminuindo
volatile uint16_t pwmValue = 0;   // Valor atual do PWM

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;

        // Define a flag de interrupção
        timerInterruptFlag = 1;
    }
}

int main(void)
{
	/*
	 * Para habilitar o clock de cada periferico leia a secao 7.3.7.APB2 peripheral clock enable register (RCC_APB2ENR)
	 * Clock dos Perifericos
	 */

	//habilite para usa o GPIOA clock

	RCC->APB2ENR |= (1 << 2);



	/*
		 * Output
		 * Para habilita um pino como saida olhe a secaoo 9.2.2 Port configuration register high (GPIOx_CRH)
		 * LED
		 *
	*/
	// OUTPUT

	/*Configure PA0 as Output Alternate Push/Pull */
	GPIOA->CRL|=GPIO_CRL_MODE0;
	GPIOA->CRL|=(GPIO_CRL_CNF0_1);
	GPIOA->CRL&=~(GPIO_CRL_CNF0_0);

	// PA1
	GPIOA->CRL |= GPIO_CRL_MODE1;
	GPIOA->CRL |= (GPIO_CRL_CNF1_1);
	GPIOA->CRL &= ~(GPIO_CRL_CNF1_0);

	// PA2
	GPIOA->CRL |= GPIO_CRL_MODE2;
	GPIOA->CRL |= (GPIO_CRL_CNF2_1);
	GPIOA->CRL &= ~(GPIO_CRL_CNF2_0);

/*******************************************************************************************************************
 * 												PWM
 * *****************************************************************************************************************
 * */
	/*Configure AF*/

	// Set Bit 0 to enable AF clock
	RCC->APB2ENR |= (1 << 0);
	/*Don't remap the pin*/
	AFIO->MAPR&=~AFIO_MAPR_TIM2_REMAP;



	/*Configure timer2*/

	// Enable clock access to timer2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;



	//Define o prescaler como 0, ou seja, o temporizador sera incrementado a cada ciclo de clock.
	TIM2->PSC = 0;
	// tempo total para um ciclo completo do PWM
	TIM2->ARR = 100;

/*
 * 	15.4.7 TIMx capture/compare mode register 1 (TIMx_CCMR1)
 * 	- 	Os canais podem ser usados em entrada (modo de captura) ou em saída (modo de comparação).
 * 		A direção de um canal é definida configurando os bits correspondentes CCxS. Todos os outros
 * 		bits deste registrador têm uma função diferente em modo de entrada e em modo de saída. Para um dado
 * 		bit, OCxx descreve sua função quando o canal está configurado em saída, ICxx descreve sua função quando
 * 		o canal está configurado em entrada. Tenha cuidado que o mesmo bit pode ter um significado diferente
 * 		para a fase de entrada e para a fase de saída.
 *
 * 	-	Esse registro define quanto tempo o sinal fica no estado "alto" antes de voltar para "baixo".
 *
 * 	-	Registro de Pré-Carga (OCxPE): Para que as configurações feitas nos registros sejam aplicadas corretamente,
 * 	 	você deve habilitar o registro de pré-carga correspondente. Isso é feito configurando o bit OCxPE no registro
 * 	 	TIMx_CCMRx. O registro de pré-carga é uma memória temporária que armazena os valores que serão usados na próxima
 * 	 	atualização do sinal PWM.
 *
 * 	OBSERVACAO: O CCMR2 usado nesse codigo tem 4 canais
 **/
	TIM2->CCMR1 |=TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1; // PA0
	TIM2->CCMR1 |=TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1; // PA1
	TIM2->CCMR2 |=TIM_CCMR2_OC3M_2|TIM_CCMR2_OC3M_1; // PA2
/*
 * 	15.4.9 TIMx capture/compare enable register (TIMx_CCER)
 *  - 	Isso é feito ajustando o bit CCxP no registro TIMx_CCER (Capture/Compare Enable Register).
 * 		Configurar esse bit define se a polaridade será alta ou baixa. Ou seja, a polaridade define
 * 		como o sinal de saída OCx será considerado "ligado" ou "desligado". Pode ser configurado para
 * 		que o sinal seja ativo alto (liga quando o valor é alto) ou ativo baixo (liga quando o valor é baixo).
 *
 *  -	 Configurar esse registrador ativa a saída OCx, permitindo que o sinal PWM gerado pelo temporizador seja
 * 		 enviado para o pino de saída correspondente.
 * 		 // Ativa a saída para o canal 4 (PB11) do PWM (TIM_CCER_CC4E).
 **/
	TIM2->CCER  |= TIM_CCER_CC1E;
	TIM2->CCER  |= TIM_CCER_CC2E;
	TIM2->CCER  |= TIM_CCER_CC3E;
	// Contador
	// Habilita o contador colocando 1 na posicao 0 . Dessa forma:  (0x1UL << (0U))
	TIM2->CR1   |= TIM_CR1_CEN;

	/*******************************************************************************************************************
	 * 											  	TIM3
	 * *****************************************************************************************************************
	 * */

    // Habilita o clock para o Timer3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configura o Timer3
    TIM3->PSC = 8000 - 1;  // Prescaler para 1 ms
    TIM3->ARR = BLINK_INTERVAL_MS - 1; // Período de 500 ms

    // Habilita a interrupção de atualização (UIE) do Timer3
    TIM3->DIER |= TIM_DIER_UIE;

    // Habilita a interrupção do Timer3 no NVIC
    NVIC_EnableIRQ(TIM3_IRQn);

    // Inicia o Timer2
    TIM3->CR1 |= TIM_CR1_CEN;

    int ledIndex = 0;  // Variável para controlar qual LED está ativo

    while(1)
    {
        if (timerInterruptFlag)
        {
            timerInterruptFlag = 0;

            // Controle do PWM para o LED atual
            if (increasing)
            {
                pwmValue++;
                if (pwmValue >= 100)
                {
                    increasing = 0;
                }
            }
            else
            {
                pwmValue--;
                if (pwmValue == 0)
                {
                    increasing = 1;

                    // Avança para o próximo LED na sequência
                    ledIndex++;
                    if (ledIndex > 2)  // Existem 3 LEDs, índice de 0 a 2
                    {
                        ledIndex = 0;
                    }
                }
            }

            // Atualiza o PWM apenas para o LED atual
            switch (ledIndex)
            {
                case 0:
                    TIM2->CCR1 = pwmValue;
                    break;
                case 1:
                    TIM2->CCR2 = pwmValue;
                    break;
                case 2:
                    TIM2->CCR3 = pwmValue;
                    break;
            }
        }
    }


}
