// =============================================================================
/**
\file    serial_flexcomm.c
\brief   Comunicação UART por meio de um periférico FlexCOMM.

\details
   Abstração de um canal de comunicação serial. Trata GPIO, UART e
   ring_buffer automaticamente. Requer que já tenha sido configurado o
   PINMUX no periférico PORT.

\b@{Histórico de Alterações:@}
    - 2019.03.25 -- Primeira versão (v1.0.0), baseada em serial.h/.c do ISD.

\author
  Renato Souza
\author
  Wagner A. P. Coimbra
*/
// =============================================================================
#include "fsl_common.h"
#include "fsl_usart.h"
#include "fsl_clock.h"
//#include "fsl_gpio.h"   <<< Não sei se algo assim será necessário. Fica como pró-memória. - original
#include "serial_flexcomm.h"



//==============================================================================
//
// Funções estáticas
//
//==============================================================================
/* DEV COMMENTS:
*		22/03/2019 - a função serial_disableIrqRX faz a desabilitação da interrupção na serial.
*					Para isso, primeiro é feita a desabilitação da interrupção de todo o flexcomm
*					Posteriormente, a interrupção do periférico específico é desabilitada, a ordem 
*					foi pensada de tal forma a desabilitar o global e depois o específico.
*/
//
// Desabilita interrupção de RX na UART
//
static inline void serial_disableIrqRX(canal_serial_t *pSerial)
{
    DisableIRQ(pSerial->flexcomm_irq);
    //UART_DisableInterrupts(pSerial->usart_base_addr, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable); - original
	USART_DisableInterrupts(pSerial->usart_base_addr, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
}
/* DEV COMMENTS:
*		22/03/2019 - a função serial_enableIrqRX é similar a serial_disableIrqRX, mas faz a 
*					habilitação da interrupção. Neste caso, primeiro é feita a habilitação
*					da interrupção do periférico específico, a qual ainda não vai funcionar 
*					pois a interrupção da flexcomm ainda não foi ativada. Posteriormente,
*					a interrupção da flexcomm inteira é habilitada, habilitando assim a 
*					interrupção do periférico específico.
*/
//
// Habilita interrupção de RX na UART, caso esteja recebendo
//
static inline void serial_enableIrqRX(canal_serial_t *pSerial)
{
    //UART_EnableInterrupts(pSerial->usart_base_addr, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable); - original
    USART_EnableInterrupts(pSerial->usart_base_addr, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);
	EnableIRQ(pSerial->flexcomm_irq);
}



//==============================================================================
//
// API
//
//==============================================================================

//------------------------------------------------------------------------------
//
// Configuração
//
//------------------------------------------------------------------------------
/* DEV COMMENTS:
*		22/03/2019 - *pSerial, *pInBuf, *pInBufArea, sizeInBuf, *pOutBuf, *pOutBufArea, sizeOutBuf,
*					useDefaultUartConfig são argumentos da inicialização do canal serial.					
*/
/**
 * @brief   Configura o hardware necessário para uma comunicação serial (GPIOs e UART).
 *          IMPORTANTE:
 *            Requer que o PinMux (periférico PORT) já tenha sido configurado.
 *
 * @param   pSerial              Instância de comunicação serial
 * @param   pInBuf               Instância de controle do buffer de recepção, vinculado à interrupção da UART
 * @param   pInBufArea           Área em RAM que representa o buffer de recepção
 * @param   sizeInBuf            Tamanho da área em RAM que representa o buffer de recepção
 * @param   pOutBuf              Instância de controle do buffer de saída, para onde são copiados os bites recebidos
 * @param   pOutBufArea          Área em RAM que representa o buffer de saída
 * @param   sizeOutBuf           Tamanho da área em RAM que representa o buffer de saída
 * @param   useDefaultUartConfig Se true, aplica a configuração default na UART. Se false, usa a configuração em PSerial
 *
 * @return  false em caso de erro ou true se for bem sucedido
 */
bool serial_init(
    canal_serial_t *pSerial,
    ringbuf_t      *pInBuf,
    uint8_t        *pInBufArea,
    uint32_t        sizeInBuf,
    ringbuf_t      *pOutBuf,
    uint8_t        *pOutBufArea,
    uint32_t        sizeOutBuf,
    bool            useDefaultUartConfig
)
{
  // Dado de entrada são válidos?
    if(    !pSerial
        || !pInBuf
        || !pInBufArea
        || !sizeInBuf
        || !pOutBuf
        || !pOutBufArea
        || !sizeOutBuf
    )
    {
        return false;
    }

    // Configura o buffer de entrada, que será usado na interrupção da UART
    if(!ringbuf_init(pInBuf, pInBufArea, sizeInBuf))
    {
        return false;
    }
    pSerial->pInBuf = pInBuf;

    // Configura o buffer de saída, que será usado no laço principal
    if(!ringbuf_init(pOutBuf, pOutBufArea, sizeOutBuf))
    {
        return false;
    }
    pSerial->pOutBuf = pOutBuf;

    //
    // Configura a UART para operar no modo default:
    //   config.baudRate_Bps = 115200U;
    //   config.parityMode = kUART_ParityDisabled;
    //   config.stopBitCount = kUART_OneStopBit;
    //   config.txFifoWatermark = 0;
    //   config.rxFifoWatermark = 1;
    //   config.enableTx = false;
    //   config.enableRx = false;
    //
    if(useDefaultUartConfig)
    {
        //UART_GetDefaultConfig(&pSerial->config); - orginal
		USART_GetDefaultConfig(&pSerial->config);
    }

    // Liga a UART e habilita recepção pela UART
    pSerial->config.enableTx = true;
    pSerial->config.enableRx = true;
    //UART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(pSerial->flexcomm_clock)); - original
	USART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(pSerial->flexcomm_clock));

    // Limpa os buffers
    ringbuf_Clear(pSerial->pInBuf);
    ringbuf_Clear(pSerial->pOutBuf);

    return true;
}
/* DEV COMMENTS:
*		22/03/2019 - Pode ser que nem todos os baud rates estejam disponíveis em todos os microcontroladores.
*/
/**
 * @brief   Configura o baudrate (aguarda terminar transmissões em andamento,
 *          corta o clock, reconfigura a UART, religa a UART e limpa os buffers)
 *          IMPORTANTE:
 *            Requer que o PinMux (periférico PORT) já tenha sido configurado.
 *
 * @param   pSerial       Instância de comunicação serial
 * @param   baudrate_bps  Novo baud rate
 *
 * @return  false em caso de erro ou true se for bem sucedido
 */
bool serial_SetBaudrate(canal_serial_t *pSerial, uint32_t baudrate_bps)
{
    if(
        !pSerial ||
        (
             baudrate_bps != 110
          && baudrate_bps != 300
          && baudrate_bps != 600
          && baudrate_bps != 1200
          && baudrate_bps != 2400
          && baudrate_bps != 4800
          && baudrate_bps != 9600
          && baudrate_bps != 14400
          && baudrate_bps != 19200
          && baudrate_bps != 28800
          && baudrate_bps != 38400
          && baudrate_bps != 56000
          && baudrate_bps != 57600
          && baudrate_bps != 115200
          && baudrate_bps != 128000
          && baudrate_bps != 153600
          && baudrate_bps != 230400
          && baudrate_bps != 256000
          && baudrate_bps != 460800
          && baudrate_bps != 921600
        )
      )
        return false;
	/* DEV COMMENTS:
	*		22/03/2019 - É possbilidade utilizar o status_t para avaliar se a inicialização da USART funcionou.
	*/
    // Aplica o novo baudrate
    USART_Deinit(pSerial->usart_base_addr);
    pSerial->config.baudRate_Bps = baudrate_bps;
    // UART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(SYS_CLK)); - original
	USART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(pSerial->flexcomm_clock));
    // Limpa os buffers
    ringbuf_Clear(pSerial->pInBuf);
    ringbuf_Clear(pSerial->pOutBuf);

    return true;
}
/* DEV COMMENTS:
*		22/03/2019 - Mudança de uart_parity_mode_t para usart_parity_mode_t
*					Mudança nos kUSART_ParityDisabled, kUSART_ParityEven e kUSART_ParityOdd
*/
/**
 * @brief   Configura a paridade (aguarda terminar transmissões em andamento,
 *          corta o clock, reconfigura a UART, religa a UART e limpa os buffers)
 *          IMPORTANTE:
 *            Requer que o PinMux (periférico PORT) já tenha sido configurado.
 *
 * @param   pSerial       Instância de comunicação serial
 * @param   parityMode    Nova paridade
 *
 * @return  false em caso de erro ou true se for bem sucedido
 */
bool serial_SetParity(canal_serial_t *pSerial, usart_parity_mode_t parityMode)
{
    if(
        !pSerial ||
        (
             parityMode != kUSART_ParityDisabled
          && parityMode != kUSART_ParityEven
          && parityMode != kUSART_ParityOdd
        )
      )
        return false;

    // Aplica a nova paridade
    USART_Deinit(pSerial->usart_base_addr);
    pSerial->config.parityMode = parityMode;
    //UART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(SYS_CLK)); - original
	USART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(pSerial->flexcomm_clock));

    // Limpa os buffers
    ringbuf_Clear(pSerial->pInBuf);
    ringbuf_Clear(pSerial->pOutBuf);

    return true;
}
/* DEV COMMENTS:
*		22/03/2019 - #if defined(FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
*					retirado pois não existe neste LPC, estava aqui pois existia no ISD.
*/
/**
 * @brief   Configura o número de stop bits (aguarda terminar transmissões em
 *          andamento, corta o clock, reconfigura a UART, religa a UART e limpa
 *          os buffers).
 *          IMPORTANTE:
 *            1. Requer que o PinMux (periférico PORT) já tenha sido configurado.
 *            2. O K22F não suporta configuração de stop bits (é fixo no
 *               hardware em 1 stop bit).
 *
 * @param   pSerial       Instância de comunicação serial
 * @param   stopBitCount  Nova paridade
 *
 * @return  false em caso de erro ou true se for bem sucedido
 */
bool serial_SetStopbit(canal_serial_t *pSerial, usart_stop_bit_count_t stopBitCount)
{
// #if defined(FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT - original
    //usart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */ - original
    if(
        !pSerial ||
        (
             stopBitCount != kUSART_OneStopBit
          && stopBitCount != kUSART_TwoStopBit
        )
      )
        return false;

    // Aplica a nova quantidade de stop bits
    USART_Deinit(pSerial->usart_base_addr);
    pSerial->config.stopBitCount = stopBitCount;
    //UART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(SYS_CLK)); - original
	USART_Init(pSerial->usart_base_addr, &pSerial->config, CLOCK_GetFreq(pSerial->flexcomm_clock));

    // Limpa os buffers
    ringbuf_Clear(pSerial->pInBuf);
    ringbuf_Clear(pSerial->pOutBuf);
// #endif // FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT - original

    return true;
}
/* DEV COMMENTS:
*		22/03/2019 - Não precisar fazer nada aqui uma vez que o serial_disableIrqRX e serial_enableIrqRX
*					ja foram revisados
*/
/**
 * @brief   Limpa o Inbuf (buffer pendurado na interrupção de recepção da UART)
 *          IMPORTANTE:
 *            - A IRQ de RX da UART fica desabilitada durante esta rotina.
 *
 * @param   pSerial    Instância de comunicação serial
 */
void serial_Clear(canal_serial_t *pSerial)
{
    // Desabilita interrupção de RX na UART
    serial_disableIrqRX(pSerial);

    ringbuf_Clear(pSerial->pInBuf);

    // Reabilita interrupção de RX na UART, se necessário
    serial_enableIrqRX(pSerial);
}



//------------------------------------------------------------------------------
//
// Leitura
//
//------------------------------------------------------------------------------

/**
 * @brief   Determina o total de bytes recebidos que estão disponíveis.
 *          IMPORTANTE:
 *            - A IRQ de RX da UART fica desabilitada durante esta rotina.
 *
 * @param   pSerial              Instância de comunicação serial
 *
 * @return  O total de bytes recebidos e disponíveis em Inbuf.
 */
int serial_Available(canal_serial_t *pSerial)
{
    // Desabilita interrupção de RX na UART
    serial_disableIrqRX(pSerial);

    // Determina o total de bytes disponíveis para leitura
    uint32_t tot = ringbuf_TotWriten(pSerial->pInBuf);

    // Reabilita interrupção de RX na UART, se necessário
    serial_enableIrqRX(pSerial);

    return tot;
}

/**
 * @brief   Transfere N bytes recebidos pela serial para o buffer de saída.
 *          IMPORTANTE:
 *            - A IRQ de RX da UART fica desabilitada durante esta rotina.
 *
 * @param   pSerial              Instância de comunicação serial
 * @param   total_to_read        Total de bytes a transferir para o buffer de saída
 *
 * @return  O total de bytes transferidos.
 */
uint32_t serial_Read(canal_serial_t *pSerial, uint32_t total_to_read)
{
    uint8_t byte;
    uint32_t total_really_read = 0;

    // Desabilita interrupção de RX na UART
    serial_disableIrqRX(pSerial);

    // Efetua a leitura
    while( !ringbuf_IsEmpty(pSerial->pInBuf) && total_to_read )
    {
        ringbuf_Read(pSerial->pInBuf, &byte, 1);
        ringbuf_Write(pSerial->pOutBuf, &byte, 1);
        total_to_read--;
        total_really_read++;
    }

    // Reabilita interrupção de RX na UART, se necessário
    serial_enableIrqRX(pSerial);

    return total_really_read;
}

/**
 * @brief   Transfere todos os bytes recebidos pela serial para o buffer de saída.
 *          IMPORTANTE:
 *            - A IRQ de RX da UART fica desabilitada durante esta rotina.
 *
 * @param   pSerial              Instância de comunicação serial
 *
 * @return  O total de bytes transferidos.
 */
uint32_t serial_ReadAll(canal_serial_t *pSerial)
{
    uint8_t byte;
    uint32_t total_really_read = 0;

    if( !ringbuf_IsEmpty(pSerial->pInBuf) )
    {
        // Desabilita interrupção de RX na UART
        serial_disableIrqRX(pSerial);

        // Efetua a leitura
        while( !ringbuf_IsEmpty(pSerial->pInBuf) )
        {
            ringbuf_Read(pSerial->pInBuf, &byte, 1);
            ringbuf_Write(pSerial->pOutBuf, &byte, 1);
            total_really_read++;
        }

        // Reabilita interrupção de RX na UART, se necessário
        serial_enableIrqRX(pSerial);
    }
    return total_really_read;
}


/* DEV COMMENTS:
*		22/03/2019 - Alteração de uint32_t para size_t, assim como requisitado pelo SDK
*/
//------------------------------------------------------------------------------
//
// Escrita
//
//------------------------------------------------------------------------------

/**
 * @brief   Transmite um buffer
 *
 * @param   pSerial     Instância de comunicação serial
 * @param   pBuf        Buffer a ser transmitido
 * @param   buf_len     Total de bytes a transmitir
 */

void serial_Write(canal_serial_t *pSerial, uint8_t *pBuf, uint32_t buf_len)
{
    //UART_WriteBlocking(pSerial->usart_base_addr, pBuf, buf_len); - original
	USART_WriteBlocking(pSerial->usart_base_addr, pBuf, buf_len);
}

/**
 * @brief   Transmite uma string null-terminated.
 *
 * @param   pSerial     Instância de comunicação serial
 * @param   str         String a ser transmitida
 */
void serial_WriteStr(canal_serial_t *pSerial, char *str)
{
    int max = SERIAL_FLEXCOMM_MAX_STRING_LENGTH;
    while(*str && max--)
    {
        USART_WriteBlocking(pSerial->usart_base_addr, (uint8_t *)str++, 1);
    }
}



//------------------------------------------------------------------------------
//
// ISRs
//
//------------------------------------------------------------------------------
/* DEV COMMENTS:
*		22/03/2019 - O código abaixo deve ser colocado no handler de interrupção 
*					do código de aplicação. Abaixo há uma diferença de implementação em relação
*					ao do exemplo do SDK, onde um if é utilizado em vez de while.
*					A lógica com if assume que sempre haverá uma interrupção quando um byte chegar
*					a medida que a logica com while assume que isso pode nao ser verdade.
*/
/**
 * @brief   ISR de recepção da UART: transfere para o buffer interno (InBuf) os bytes recebidos
 *          no buffer da UART.
 *          IMPORTANTE:
 *            Execute esta rotina a partir da ISR da UART.
 *
 * @param   pSerial     Instância de comunicação serial
 */
void serial_FLEXCOMMn_IRQHandler(canal_serial_t *pSerial)
{
    uint8_t data;

    // Se chegou novo byte...
    while ((kUSART_RxFifoNotEmptyFlag | kUSART_RxError | kUSART_RxFifoFullFlag) & USART_GetStatusFlags(pSerial->usart_base_addr))
    {
        // ... lê esse byte ...
        data = USART_ReadByte(pSerial->usart_base_addr);

        // ... e o adiciona no buffer circular de entrada
        ringbuf_WriteByte(pSerial->pInBuf, data);
    }
}



//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------

#if defined(LOGICALIS_DEBUG_SERIAL_FLEXCOMM) && (LOGICALIS_DEBUG_SERIAL_FLEXCOMM)

#include <stdio.h>

#define DEBUG_SERIAL_BUFFER_SIZE_BYTES   64
#define DEBUG_CHAR_CR                    0x0D
#define DEBUG_CHAR_LF                    0x0A

//
// Instância de serial para o teste
//		Com o intuito de testar as funções serial_SetBaudrate, serial_SetParity e serial_SetStopbit,
// 		o baudrate, parity e stopbit são inicializados "errados" e são corrigidos usando as referidas
//		funções.
//
canal_serial_t debug_serial_instance =
{
    .usart_base_addr = USART0,
	.flexcomm_clock= kCLOCK_BusClk,
    .flexcomm_irq = FLEXCOMM0_IRQn,
    .config =
    {
        .baudRate_Bps = 9600U,
        .parityMode = kUSART_ParityOdd,
		.stopBitCount = kUSART_TwoStopBit,
		.bitCountPerChar = kUSART_8BitsPerChar,
		.loopback = false,
        .txWatermark = 0,
        .rxWatermark = 1,
        .enableTx = true,
        .enableRx = true,
    },
    .pInBuf = (ringbuf_t *)0,
    .pOutBuf = (ringbuf_t *)0
};
ringbuf_t inbuf; // Buffer circular "inbuf"
uint8_t inbuf_area[DEBUG_SERIAL_BUFFER_SIZE_BYTES]; // Área de armazenamento em RAM
ringbuf_t outbuf; // Buffer circular "outbuf"
uint8_t outbuf_area[DEBUG_SERIAL_BUFFER_SIZE_BYTES]; // Área de armazenamento em RAM

//
// Implementa um servidor echo (replica na UART tudo que for recebido)
//
void debug_serial_echoServer()
{
    bool Ok = true;
    int bytes_lidos;
    uint8_t carac;

    //
    // Configuração
    //
    Ok = serial_init(
        &debug_serial_instance,
        &inbuf,
        inbuf_area,
        DEBUG_SERIAL_BUFFER_SIZE_BYTES,
        &outbuf,
        outbuf_area,
        DEBUG_SERIAL_BUFFER_SIZE_BYTES,
        false
    );
    if(!Ok)
    {
        printf("Lib 'serial.h/.c': falhou em serial_init()\nTeste encerrado\n");
        return;
    }

    // Acertando a configuração usando as funções especificas
    serial_SetBaudrate(&debug_serial_instance,115200U);
    serial_SetParity(&debug_serial_instance, kUSART_ParityDisabled);
    serial_SetStopbit(&debug_serial_instance, kUSART_OneStopBit);

    while(1)
    {

        if(serial_Available(&debug_serial_instance))
        {
            bytes_lidos = serial_ReadAll(&debug_serial_instance);
            while(!ringbuf_IsEmpty(&outbuf))
            {
                Ok = ringbuf_ReadByte(&outbuf, &carac);
                if(!Ok)
                {
                    printf("Lib 'serial.h/.c': Falhou ao ler o OutBuf...\n");
                    return;
                }
                //printf("%c", carac);

                serial_Write(&debug_serial_instance, &carac, 1);
            }
        }
    }
}

#endif // LOGICALIS_DEBUG_SERIAL_FLEXCOMM
