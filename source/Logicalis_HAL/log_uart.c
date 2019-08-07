// =============================================================================
/**
\file    log_uart.c
\brief   Apoio a depurações por meio de mensagens enviadas via UART.

\details
  Apoio a depurações em runtime. Como o uso de printf() pela JTAG deixou as
  temporizações caóticas em testes feitos com um Cortex-M4 (como se todas
  as interrupções fossem desabilitadas durante o printf), foi criada esta
  biblioteca, que permite enviar mensagens de depuração por meio de uma
  UART.
  O custo computacional é muito menor que o da função printf() e todos os
  timers e interrupções do sistema são preservados.

\b@{Histórico de Alterações:@}
    - 2019.03.26 -- Primeira versão (v1.0.0), baseada em log_uart1.h/c

\author
  Renato Souza
\author
  Wagner A. P. Coimbra
*/
// =============================================================================
#if defined (LOGICALIS_LOG_UART_ACTIVE) && (LOGICALIS_LOG_UART_ACTIVE)
#include "fsl_common.h"
#include "fsl_usart.h"
#include "fsl_clock.h"
#include "serial_flexcomm.h"
#include "log_uart.h"
#include <stdio.h>

//==============================================================================
//
// API
//
//==============================================================================
#define HEXSTRING_SIZE       5 // Sem o terminador nulo
//------------------------------------------------------------------------------
//
// Configuração
//
//------------------------------------------------------------------------------

//
// Configura a USART DE ACORDO COM AS VARIAVEIS DEFINIDAS NO .h
//
/* DEV COMMENTS:
*		25/03/2019 - Implementação inicial não irá generalizar a interface serial uasada para log.
*					Inicialmente será utilizado a USART0 da Flexcomm0
*/
char *byteToStringHEX(uint8_t byte);
void log_init()
{
    usart_config_t usart_config;

    // Configura a USART para operar no modo default, posteriormente seta os valores customizados:
    //   .baudRate_Bps = 115200U;
    //   .parityMode = kUSART_ParityDisabled;
    //   .stopBitCount = kUSART_OneStopBit;
    //   .txFifoWatermark = 0;
    //   .rxFifoWatermark = 1;
    //   .enableTx = false;
    //   .enableRx = false;
    USART_GetDefaultConfig(&usart_config);

    // Habilita a transmissão apenas
    usart_config.enableTx = true;
    usart_config.enableRx = false;

    // Configuração específica
    usart_config.baudRate_Bps = LOG_USART_BAUD;       		/*!< USART baud rate  */
    usart_config.parityMode = LOG_USART_PARITY;       		/*!< Parity mode, disabled (default), even, odd */
    usart_config.stopBitCount = LOG_USART_STOP;  			/*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
    usart_config.bitCountPerChar = LOG_USART_DATA_LENGTH;	/*!< Data length - 7 bit, 8 bit  */

    // Liga a UART1
    USART_Init(LOG_USART_FLEXCOMM, &usart_config, CLOCK_GetFreq(LOG_FLEXCOMM_CLOCK));
}


//------------------------------------------------------------------------------
//
// Escrita na USART
//
//------------------------------------------------------------------------------

//
// Escreve um buffer USART configurada, convertendo cada byte para uma string hexadecimal
//
void log_writeBufAsHexString(uint8_t *pBuf, int16_t size)
{
    while(size--)
        // log_writeBuf((uint8_t *)byteToStringHEX(*pBuf++), HEXSTRING_SIZE); --
    	log_writeBuf((uint8_t *)byteToStringHEX(*pBuf++), HEXSTRING_SIZE);
}

//
// Escreve uma string na USART configurada
//
void log_writeString(char *str)
{
    while(*str)
        log_write((uint8_t) *str++);
}

//
// Escreve 0 valor de um bool na USART configurada
//
void log_writeBool(bool value)
{
    if(value)
    {
        log_writeString("true");
    }
    else
    {
        log_writeString("false");
    }
}

//
// Escreve um byte na USART configurada
//
void log_write(uint8_t byte)
{
    USART_WriteBlocking(LOG_USART_FLEXCOMM, &byte, 1);
}

//
// Escreve um buffer na USART configurada
//
void log_writeBuf(uint8_t *pBuf, int16_t size)
{
    USART_WriteBlocking(LOG_USART_FLEXCOMM, pBuf, size);
}

//
// Escreve um caractere na USART configurada
//
void log_writeChar(char c)
{
    log_write((uint8_t) c);
}

//
// Escreve um UINT32 na USART configurada
//
#define LOG_WRITE32_BUF_SIZE  16
void log_writeU32(uint32_t val)
{
    char buf[LOG_WRITE32_BUF_SIZE];
    int r;
    int i = 0;

    //
    // Decompõe os algarismos
    //
    do
    {
        r = val % 10;
        val /= 10;
        buf[i++] = (char)r + '0';
    }while(val && i<LOG_WRITE32_BUF_SIZE);

    //
    // Imprime os algarismos
    //
    if(i >= LOG_WRITE32_BUF_SIZE)
    {
        log_writeChar('?');
    }
    else
    {
        while(i)
        {
            log_writeChar(buf[--i]);
        }
    }
}



//------------------------------------------------------------------------------
//
// Apoio
//
//------------------------------------------------------------------------------

//
// Tamanho do buffer para converter de número binário para string hexadecimal
//
#define HEXSTRING_BUF_SIZE   6


//
// Converte um valor binário para uma string no formato "[XX] ",
// onde XX é o valor em hexadecimal
//
char *byteToStringHEX(uint8_t byte)
{
    //
    // Buffer usado para converter de número binário para string hexadecimal
    //
    static char s_str[HEXSTRING_BUF_SIZE] = "[XX] ";

    s_str[2] = byte & 0x0F;    // LS-Nibble
    s_str[1] = byte>>4 & 0x0F; // MS-Nibble

    s_str[2] += (s_str[2] > 9)  ? 55  : '0'; // Caractere que representa o LSB no intervalo '0' a 'F'
    s_str[1] += (s_str[1] > 9)  ? 55  : '0'; // Caractere que representa o MSB no intervalo '0' a 'F'

    return s_str;
}



//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------

#if defined (LOGICALIS_DEBUG_LOG_UART) && (LOGICALIS_DEBUG_LOG_UART)
static void debug_log_uart_messages()
{
	// Configuração da interface serial e parâmetros dentro da biblioteca.
	log_init();
	uint8_t char_buff[7]={56,57,65,97,32,13,10};//"89Aa \r\n";
	log_writeBufAsHexString(char_buff,7);

	log_writeString("\r\nTestando writeString \r\n");

	log_writeBool(true);
	log_write(13); //return
	log_write(10); //new line

	log_writeBool(false);
	log_write(13); //return
	log_write(10); //new line

	log_write(65); //A em ASCII
	log_write(13); //return
	log_write(10); //new line

	uint8_t buffer[7] = {69,85,71,69,78,73,79}; //EUGENIO in ASCII
	log_writeBuf(buffer,7);
	log_write(13); //return
	log_write(10); //new line

	log_writeChar('f');
	log_write(13); //return
	log_write(10); //new line

	log_writeU32(1080);
	log_write(13); //return
	log_write(10); //new line
}

static void debug_byteToStringHEX()
{
    char *msg_erro = "<NULL>";
    char *result = (void *) 0;
    uint8_t value = 0;

    // Valida passar um CHAR
    // Resultado é o valor em hexa de acordo com ASCII table
    // NOTE: Os testes passando uma string ou char para o value irá gerar warnings esperados
    value = '8';
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "38");
    printf("\n   byteToStringHEX:  \t%s", result);


    value = 'A';
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "41");
    printf("\n   byteToStringHEX:  \t%s", result);


    value = 'a';
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "61");
    printf("\n   byteToStringHEX:  \t%s", result);

    // O teste abaixo irá gerar um warning, o que mostra
    value = '*';
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "2A");
    printf("\n   byteToStringHEX:  \t%s", result);


    value = 57;
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "39");
    printf("\n   byteToStringHEX:  \t%s", result);

    value = 111;
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;

    printf("\n   Esperado:  \t%s", "6F");
    printf("\n   byteToStringHEX:  \t%s", result);

    // O teste abaixo gera um Warning esperado:
    //   warning: large integer implicitly truncated to unsigned type [-Woverflow]
    value = 666;
    result = byteToStringHEX(value);
    if (!result)
        result = msg_erro;
    // Este teste mostra o resultador quando o value é maior que (2^8-1)
    printf("\n   Esperado:  \t%s", "29A");
    printf("\n   byteToStringHEX:  \t%s", result);
}
void debug_log_uart()
{
	debug_log_uart_messages();
	debug_byteToStringHEX();
}

#endif // LOGICALIS_DEBUG_LOG_UART



#endif // LOGICALIS_LOG_UART_ACTIVE
