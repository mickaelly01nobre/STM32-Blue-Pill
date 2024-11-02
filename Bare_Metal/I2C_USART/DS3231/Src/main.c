#include "stm32f1xx.h"
#include "i2c.h"
#include "uart.h"
#include "stdio.h"
#include "stdlib.h"

uint8_t rtc_data[3];

/* O DS3231 usa o formato BCD (Binary-Coded Decimal), que representa cada dígito decimal com 4 bits
 * (por exemplo, 25 é 0010 0101). Funções BCD são necessárias para converter os valores entre BCD e binário,
 * facilitando cálculos e exibições dos dados de tempo.
 *
 * */

int bcd_to_decimal(unsigned char x) {
    return x - 6 * (x >> 4);
}



int main(void)
{
	uart2_init();
	i2c_init();
	i2c1_scan_bus();
	// Inicializa a semente do gerador de números aleatórios com valor fixo.
	srand(1);
	while(1)
	{
		/* Le os valores de horas, minutos e segundos do DS3231 (endereço 0x68), começando no registrador 0x00
		 * e armazena esses valores em rtc_data. O valor 3 indica que três bytes (segundos, minutos e horas) são lidos.
		 */
		i2c1_readMemoryMulti((uint8_t)0x68,(uint8_t)0x00,rtc_data,(uint8_t)3);
		for (uint8_t i=0;i<3;i++)
		{
			rtc_data[i]=bcd_to_decimal(rtc_data[i]);
		}
		/* Criamos a função de escrita com multiplos que verifica se os segundos (rtc_data[0]) forem igual a 5, se for igual cria-se 			um array data_s postulando valores aleatórios para minutos e horas,
		 * e escreve-se esses valores de volta ao DS3231 usando i2c1_writeMemoryMulti().
		 * para ativa essa opção de escrita de mútiplos bytes basta descomentar o if abaixo.
		 * */
		/*
		if(rtc_data[0]==5)
		{
			uint8_t data_s[3]={0,rand()%20,rand()%10};
			i2c1_writeMemoryMulti(0x68,0x00,data_s,3);
		}
		*/

		printf("RTC time is: %d:%d:%d\r\n", rtc_data[2],rtc_data[1],rtc_data[0]);
		for (int i=0;i<100000;i++);
	}
}
