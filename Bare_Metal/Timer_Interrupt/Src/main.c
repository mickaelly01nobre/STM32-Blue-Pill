#include <stdint.h>
#include "stm32f1xx.h"

/* Define o estado dos LEDs */
typedef enum {
    LED_STATE_RED,
    LED_STATE_GREEN,
    LED_STATE_BLUE,
    LED_STATE_CYAN,
    LED_STATE_MAGENTA,
    LED_STATE_YELLOW,
    LED_STATE_COUNT // Total de estados LED
} LEDState;

/* Define o intervalo de tempo para cada piscada */
#define BLINK_INTERVAL_MS 500
#define BLINK_COUNT 2 // Número de piscadas consecutivas para cada cor

typedef struct {
    LEDState currentLEDState;
    uint32_t blinkCounter; // Contador de piscadas
    uint32_t blinkState; // 0 - LED apagado, 1 - LED aceso
    uint32_t stateChangeCounter; // Contador para mudanças de cor
} LEDControl;

/* Instância do controle dos LEDs */
static LEDControl ledControl = {
    .currentLEDState = LED_STATE_RED,
    .blinkCounter = 0,
    .blinkState = 1,
    .stateChangeCounter = 0
};

/* Flag de interrupção */
volatile uint8_t timerInterruptFlag = 0;

void turnOffAllLEDs(void)
{
    GPIOB->ODR &= ~(1 << 8); // Apaga LED vermelho
    GPIOB->ODR &= ~(1 << 9); // Apaga LED verde
    GPIOB->ODR &= ~(1 << 10); // Apaga LED azul
}

void updateLEDs(void)
{
    // Verifica o estado do LED atual e atualiza a saída
    uint16_t ledOutput = 0;
    switch (ledControl.currentLEDState)
    {
        case LED_STATE_RED:
            ledOutput = (1 << 8); // LED vermelho
            break;
        case LED_STATE_GREEN:
            ledOutput = (1 << 9); // LED verde
            break;
        case LED_STATE_BLUE:
            ledOutput = (1 << 10); // LED azul
            break;
        case LED_STATE_CYAN:
            ledOutput = (1 << 9) | (1 << 10); // LED ciano (verde + azul)
            break;
        case LED_STATE_MAGENTA:
            ledOutput = (1 << 8) | (1 << 10); // LED magenta (vermelho + azul)
            break;
        case LED_STATE_YELLOW:
            ledOutput = (1 << 8) | (1 << 9); // LED amarelo (vermelho + verde)
            break;
        default:
            ledOutput = 0; // Nenhuma cor
            break;
    }

    if (ledControl.blinkState == 1)
    {
        // LED aceso
        GPIOB->ODR = ledOutput;
        ledControl.blinkCounter++;
        if (ledControl.blinkCounter >= BLINK_COUNT) // Pisca o número de vezes definido
        {
            ledControl.blinkCounter = 0;
            ledControl.blinkState = 0; // Mudar para LED apagado
        }
    }
    else
    {
        // LED apagado
        turnOffAllLEDs();
        ledControl.blinkCounter++;
        if (ledControl.blinkCounter >= BLINK_COUNT) // Pisca o número de vezes definido
        {
            ledControl.blinkCounter = 0;
            ledControl.stateChangeCounter++;
            if (ledControl.stateChangeCounter >= 2) // Mudar o estado após 2 piscadas (em LED apagado)
            {
                ledControl.stateChangeCounter = 0;
                ledControl.currentLEDState++;
                if (ledControl.currentLEDState >= LED_STATE_COUNT)
                {
                    ledControl.currentLEDState = LED_STATE_RED; // Volta para vermelho se passar da última cor
                }
                // Garanta que todos os LEDs sejam apagados antes de iniciar o próximo ciclo
                turnOffAllLEDs();
                ledControl.blinkState = 1; // Assegura que a nova cor comece com o LED aceso
            }
            else
            {
                ledControl.blinkState = 1; // Continuar piscando a mesma cor
            }
        }
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        // Define a flag de interrupção
        timerInterruptFlag = 1;
    }
}

int main(void)
{
    // Habilita o clock para GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Configura os pinos GPIOB 8, 9 e 10 como saída
    GPIOB->CRH &= 0xFFFF0000; // Limpa os bits de configuração dos pinos 8, 9 e 10
    GPIOB->CRH |= 0x00002222; // Configura os pinos 8, 9 e 10 como saída push-pull, máxima velocidade de 2 MHz

    // Habilita o clock para o Timer2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Configura o Timer2
    TIM2->PSC = 8000 - 1;  // Prescaler para 1 ms
    TIM2->ARR = BLINK_INTERVAL_MS - 1; // Período de 500 ms

    // Habilita a interrupção de atualização (UIE) do Timer2
    TIM2->DIER |= TIM_DIER_UIE;

    // Habilita a interrupção do Timer2 no NVIC
    NVIC_EnableIRQ(TIM2_IRQn);

    // Inicia o Timer2
    TIM2->CR1 |= TIM_CR1_CEN;

    while (1)
    {
        if (timerInterruptFlag)
        {
            // Atualiza o estado dos LEDs a cada interrupção
            updateLEDs();

            // Limpa a flag de interrupção
            timerInterruptFlag = 0;
        }
    }
}
