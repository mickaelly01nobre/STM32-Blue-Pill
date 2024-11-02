
#include "i2c.h"
#include "stm32f1xx.h"

/*
 * Esta função configura a interface I2C no microcontrolador STM32. Ela faz a configuração dos pinos GPIOB 6 e 7,
 * que são usados como SCL (clock) e SDA (dados) respectivamente, para operar no modo "open-drain" alternativo.
 * Além disso, habilita o relógio para o I2C1 e configura o periférico I2C para operar com uma frequência de 8 MHz,
 * ajustando também o tempo de subida (TRISE) e os registros de controle do I2C (CCR e CR1).
 *
 * */

void i2c_init()
{
	/*Habilita o GPIOB*/
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;

	/*habilitia o PB6 como output 50MHz*/
	// SDL
	GPIOB->CRL|=GPIO_CRL_MODE6;
	GPIOB->CRL|=GPIO_CRL_CNF6;

	/*Habilita PB7 para output 50MHz*/
	// SDA
	GPIOB->CRL|=GPIO_CRL_MODE7;
	GPIOB->CRL|=GPIO_CRL_CNF7;

	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;

	/*Habilita o clock  I2C1*/
	RCC->APB1ENR|=RCC_APB1ENR_I2C1EN;


	/*Chama o periferico com o  clock is 8MHz*/
	I2C1->CR2&=~(I2C_CR2_FREQ);
	I2C1->CR2|=(8<<I2C_CR2_FREQ_Pos);
	// habilita o rise time*/
	I2C1->TRISE=9;

	I2C1->CCR|=0x28;

	I2C1->CR1|=I2C_CR1_PE;
}


static void delay(uint32_t delay)
{
	for (volatile int i=0;i<1000;i++);
}

/*
 * Esta função varre os endereços do barramento I2C (0 a 127) para encontrar dispositivos conectados. Ela envia um "start condition"
 * , transmite o endereço do dispositivo e verifica se houve resposta, indicando que um dispositivo está presente nesse endereço.
 * Se encontrado, o endereço do dispositivo é impresso via printf().
 * */
void i2c1_scan_bus(void)
{
	int a=0;
	for (uint8_t i=0;i<128;i++)
	{
		I2C1->CR1 |= I2C_CR1_START;
		while(!(I2C1->SR1 & I2C_SR1_SB));
		I2C1->DR=(i<<1|0);
		while(!(I2C1->SR1)|!(I2C1->SR2)){};
		I2C1->CR1 |= I2C_CR1_STOP;
		delay(100);      //o minimo de espera é 40 uS, mas é interessante deixar 100uS
		a=(I2C1->SR1&I2C_SR1_ADDR);
		if (a==2)
		{
			printf("Found I2C device at address 0x%X (hexadecimal), or %d (decimal)\r\n",i,i);
		}
	}
}

/* Esta função escreve um único byte em uma memória de um dispositivo I2C. Primeiro, ela envia o endereço do dispositivo (saddr),
 * depois o endereço de memória (maddr), e por último escreve o dado (data) no dispositivo I2C. Ela também lida com as condições de
 * início e término da comunicação I2C.
 *
 * */

void i2c1_MemoryWrite_Byte(uint8_t saddr, uint8_t maddr, uint8_t data)
{
	while(I2C1->SR2&I2C_SR2_BUSY){;}
	I2C1->CR1|=I2C_CR1_START;
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	I2C1->DR = saddr<< 1;
	while(!(I2C1->SR1&I2C_SR1_ADDR)){;}
	(void)I2C1->SR2;
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}
	I2C1->DR = maddr;
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_BTF));
	I2C1->CR1 |=I2C_CR1_STOP;
}


/* Esta função escreve um único byte de dados para um dispositivo I2C no endereço especificado por saddr.
 * Similar à função anterior, mas sem o endereço de memória, útil para dispositivos que não utilizam endereçamento
 * interno de memória.
 *
 *
 * */
void i2c1_Write_Byte(uint8_t saddr, uint8_t data)
{
	while(I2C1->SR2&I2C_SR2_BUSY){;}          // espera ate que o barramento nao esteja ocupado (Habilita o bit 1 "busy")
	I2C1->CR1|=I2C_CR1_START;                  // gera o inicial (habilita o bit 8)
	while(!(I2C1->SR1&I2C_SR1_SB)){;}         // espera ate o inicio seja gerado (Habilita o bit 0)
	I2C1->DR = saddr<< 1;                 	 // Após o início ser gerado, envie o endereço do escravo deslocado para a esquerda em 1 bit.
	while(!(I2C1->SR1&I2C_SR1_ADDR)){;}      // aguarde ate o enderenco seja definido (Habilita o bit 1)
	(void)I2C1->SR2; 						  // Limpa SR2
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}       //Aguarde até que o buffer de transmissão esteja vazio verificando o bit TXE no registro SR1:
	I2C1->DR = data;                        /* send memory address*/
	while (!(I2C1->SR1 & I2C_SR1_BTF));      /*wait until transfer finished*/
	I2C1->CR1 |=I2C_CR1_STOP;				/*Generate Stop*/
}

/* Lê um único byte de uma memória interna de um dispositivo I2C. A função envia o endereço do dispositivo (saddr),
 * o endereço de memória (maddr), e lê o dado da memória para a variável data
 * */
void i2c1_readMemoryByte(uint8_t saddr,uint8_t maddr, uint8_t *data)
{
	while(I2C1->SR2&I2C_SR2_BUSY){;}
	I2C1->CR1|=I2C_CR1_START;
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	I2C1->DR=saddr<<1;
	while(!(I2C1->SR1&I2C_SR1_ADDR)){;}
	(void)I2C1->SR2;
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}
	I2C1->DR=maddr;
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}
	I2C1->CR1|=I2C_CR1_START;
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	I2C1->DR=saddr<<1|1;
	while(!(I2C1->SR1&I2C_SR1_ADDR)){;}
	I2C1->CR1&=~I2C_CR1_ACK;
	(void)I2C1->SR2;
	I2C1->CR1|=I2C_CR1_STOP;
	while(!(I2C1->SR1&I2C_SR1_RXNE)){;}
	*data=I2C1->DR;
}
/* Lê um único byte de um dispositivo I2C sem especificar um endereço de memória. É útil para dispositivos que respondem com
 * dados diretamente sem um esquema de endereçamento de memória interno.
 *
 *
 *   - 	Exemplo de uso: enviar comandos para um dispositivo I2C como um sensor, que pode não ter um esquema de endereçamento
 * 		interno de memória, ou quando se quer apenas transmitir dados diretamente.
 * */

void i2c1_readByte(uint8_t saddr, uint8_t *data)
{
	while(I2C1->SR2&I2C_SR2_BUSY){;}
	I2C1->CR1|=I2C_CR1_START;
	while(!(I2C1->SR1&I2C_SR1_SB)){;}
	I2C1->DR=saddr<<1|1;
	while(!(I2C1->SR1&I2C_SR1_ADDR)){;}
	I2C1->CR1&=~I2C_CR1_ACK;
	(void)I2C1->SR2;
	I2C1->CR1|=I2C_CR1_STOP;
	while(!(I2C1->SR1&I2C_SR1_RXNE)){;}
	*data=I2C1->DR;

}


/* Para ler múltiplos bytes, primeiro enviamos a condição de início, depois o endereço do escravo com bit de gravação e depois o
 * local de memória no nosso caso (segundos local de memória 0x00). Depois disso, um início de repetição deve ser gerado e enviar
 *  o endereço do escravo com bit de leitura, depois disso, habilitamos o reconhecimento (ACK). Continuamos lendo (no nosso caso 3)
 *  até que reste um byte, então desabilitamos o reconhecimento (NAK). A figura abaixo mostra a sequência de leitura.
 *
 *
 *   -	Exemplo de uso: escrever em uma posição específica de memória em uma EEPROM, ou configurar um registrador
 * 		específico de um sensor que tem múltiplos registradores endereçáveis.
 *
 *
 * */

/* A função recebe quatro argumentos:
 * 	- Endereço do escravo;
 * 	- Endereço de memória;
 * 	- Ponteiro para um buffer que contém os dados a serem gravados;
 * 	- Comprimento do buffer;
 *
 * */

void i2c1_readMemoryMulti(uint8_t saddr,uint8_t maddr, uint8_t *data, uint8_t length)
{
	// 26.6.7 I2C Status register 1 (I2C_SR2)
	// Dentro da função, começamos esperando até que o barramento esteja livre pelo bit pronto e ocupado do registrador SR1
	while (I2C1->SR2 & I2C_SR2_BUSY){;} // (Habilita o bit 1 "busy")

	//26.6.1 I2C Control register 1 (I2C_CR1)
	I2C1->CR1|=I2C_CR1_START; // Após o barramento ser liberado, envie a condição de início definindo o bit start em CR1 como 1,
	while(!(I2C1->SR1 & I2C_SR1_SB)){;} //  Aguarde até que a condição inicial seja gerada
	I2C1->DR=saddr<<1; //  envie o endereço do escravo deslocado para a esquerda em 1 bit

	//26.6.6 I2C Status register 1 (I2C_SR1)
	while(!(I2C1->SR1 & I2C_SR1_ADDR)){;} // Aguarde até que o endereço seja correspondido verificando o bit ADDR no registro SR1
	(void)I2C1->SR2; // Limpar registro SR2

	//26.6.6 I2C Status register 1 (I2C_SR1)
	while(!(I2C1->SR1&I2C_SR1_TXE)){;}// Aguarde até que o buffer de transmissão esteja vazio verificando o bit TXE no registro SR1
	I2C1->DR = maddr; // Envie o endereço de memória

	while(!(I2C1->SR1&I2C_SR1_TXE)){;}// Aguarde até que o buffer de transmissão esteja vazio

	/* Para mudar a direção de gravação para leitura, precisamos enviar a condição de reinicialização seguida do envio do endereço
	 * do escravo no modo de leitura.
	 *
	 * */
	I2C1->CR1|=I2C_CR1_START; // Para gerar a condição de reinicialização, gere outra condição de início
	while(!(I2C1->SR1 & I2C_SR1_SB)){;} // Aguarde até que a condição inicial seja gerada
	I2C1->DR=saddr<<1|1;  // envie o endereço salve com a operação de leitura
	while(!(I2C1->SR1 & I2C_SR1_ADDR)){;} // Aguarde até que o endereço seja correspondido
	(void)I2C1->SR2; // Limpar registro SR2

	// 26.6.1 I2C Control register 1 (I2C_CR1)
	/* Esse bit controla se o dispositivo enviará um sinal de reconhecimento (ACK) depois de receber um byte de dados ou endereço
	 * válido. Se estiver em 1, o dispositivo envia o ACK, indicando que recebeu os dados corretamente. Se estiver em 0, ele não
	 * envia o ACK, o que pode ser usado para indicar que o dado não foi aceito.
	 * */
	I2C1->CR1|=I2C_CR1_ACK; // Habilitar reconhecimento no registro CR1
	while(length>0U) // Caso tenhamos apenas um byte restante, devemos fazer o seguinte
	{
		if(length==1U)
		{
			I2C1->CR1&=~I2C_CR1_ACK; // Desabilitar reconhecimento
			I2C1->CR1|=I2C_CR1_STOP; // Gerar parada
			while(!(I2C1->SR1&I2C_SR1_RXNE)){;} // Aguarde até que o bit de recebimento seja definido
			*data++=I2C1->DR; // Armazena o último byte recebido no buffer
			break;
		}
		else
		{
			while(!(I2C1->SR1&I2C_SR1_RXNE)){;} // Aguarde até que o bit de recebimento seja definido
			(*data++)=I2C1->DR; // armazenar os dados no buffer e incrementar o contador de buffer
			length--; // decrementar o contador

		}


	}

}
/* A função recebe quatro argumentos:
 * 	- Endereço do escravo;
 * 	- Endereço de memória;
 * 	- Ponteiro para um buffer que contém os dados a serem gravados;
 * 	- Comprimento do buffer;
 *
 * */

void i2c1_writeMemoryMulti(uint8_t saddr,uint8_t maddr, uint8_t *data, uint8_t length)
{
	// 26.6.7 I2C Status register 1 (I2C_SR2)
	while (I2C1->SR2 & I2C_SR2_BUSY);           // espera ate que o barramento nao esteja ocupado (Habilita o bit 1 "busy")

	//26.6.1 I2C Control register 1 (I2C_CR1)
	// apos o barramento ser liberado, envia a condicao inicial
	I2C1->CR1 |= I2C_CR1_START;                 // gera o inicial (habilita o bit 8)

	//26.6.6 I2C Status register 1 (I2C_SR1)

	/*Esse bit (SB) indica se o dispositivo master em um barramento I2C iniciou a comunicação. Quando ele é 1, significa que o
	 * "start condition" foi gerado, ou seja, o master está pronto para começar a transmissão de dados. Para continuar o
	 * processo, o software precisa limpar esse bit lendo o status (SR1) e escrevendo um dado (DR).
	 * */
	while (!(I2C1->SR1 & I2C_SR1_SB)){;}		// espera ate o inicio seja gerado (Habilita o bit 0)

	/* A razão por trás da mudança é que o endereço pega
	 * o bit1 para o bit7 do registrador de dados e o bit0 é usado para operação de leitura/gravação.
	 *  Quando o bit0 é 0, a operação é de gravação e quando é 1, significa operação de leitura.
	 * */
	I2C1->DR = saddr<< 1;                 	 	// Após o início ser gerado, envie o endereço do escravo deslocado para a esquerda em 1 bit.

	//26.6.6 I2C Status register 1 (I2C_SR1)
	/* -	Esse bit (ADDR) basicamente indica se o endereço foi enviado e aceito (modo master) ou se o endereço que o dispositivo recebeu
	 * 		corresponde ao seu próprio (modo slave).  Ele ajuda o dispositivo a saber se a comunicação I2C está prosseguindo corretamente.
	 *
	 * -	Esse bit é limpo (ou seja, resetado) automaticamente quando o software lê o registro SR1 seguido pelo SR2, ou quando PE=0.
	 *
	 * */
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){;}       // aguarde ate o enderenco seja definido (Habilita o bit 1)
	(void) I2C1->SR2; 						     // Limpa SR2

	// 26.6.6 I2C Status register 1 (I2C_SR1)
	/* Esse bit (TXE) indica se o registro de dados, que armazena os dados a serem enviados, está vazio. Quando está vazio (bit em 1),
	 * o dispositivo está pronto para enviar mais dados. Se o registro ainda contém dados (bit em 0), ele ainda está ocupado
	 * transmitindo. Esse bit é automaticamente resetado quando novos dados são enviados, ou quando a comunicação é iniciada ou
	 * finalizada.
	 * */
	while (!(I2C1->SR1 & I2C_SR1_TXE));           //Aguarde até que o buffer de transmissão esteja vazio verificando o bit TXE no registro SR1:
	I2C1->DR = maddr;                      		// Envie o endereço de memória

	// 26.6.6 I2C Status register 1 (I2C_SR1)
	while (!(I2C1->SR1 & I2C_SR1_TXE));           // Aguarde até que o buffer de transmissão esteja vazio verificando o bit TXE no registro SR1
	for (uint8_t i=0;i<length;i++)
	{

		I2C1->DR=data[i]; 			//Agora, usando o loop for, enviaremos os dados um após o outro

		/*Esse bit BTF indica quando a transferência de um byte foi concluída com sucesso, seja durante a recepção ou transmissão.
		 * Se o dispositivo estiver recebendo dados, o bit é ativado quando o byte foi recebido, mas ainda não foi lido.
		 * Se estiver transmitindo, o bit é ativado quando o próximo byte está pronto para ser enviado, mas o dado ainda não foi
		 * escrito no registro de dados. Para continuar a operação, o software precisa limpar esse bit, lendo ou escrevendo no
		 * registro de dados.
		 * */
		while (!(I2C1->SR1 & I2C_SR1_BTF));
	}
	I2C1->CR1 |= I2C_CR1_STOP; //Depois que todos os dados forem transmitidos, envie a condição de parada
}
