// =============================================================================
/**
\file    serial_flexcomm.h
\brief   Comunicação UART por meio de um periférico FlexCOMM.

\details
   Abstração de um canal de comunicação serial. Trata UART e
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
#ifndef H_SERIAL_FLEXCOMM
#define H_SERIAL_FLEXCOMM

#include <stdint.h>
#include <stdbool.h>
#include "fsl_usart.h"
#include "ring_buffer.h"      // Esta lib implementa um buffer circular em RAM
#include "fsl_clock.h"

//==============================================================================
//
// Constantes
//
//==============================================================================

/**
 * @brief   Alterar o valor para "1" habilita as rotinas de depuração da serial.
 */
#define LOGICALIS_DEBUG_SERIAL_FLEXCOMM   1
/* DEV COMMENTS:
*		21/03/2019 - define SERIAL_FLEXCOMM_MAX_STRING_LENGTH foi adicionado depois que o fizemos a leitura da biblioteca serial
*					e na função serial_WriteStr tem um valor max que representa o tamanho máximo de uma string a ser transmitida.
*					Para deixar o código com maior qualidade, foi definida essa variável global.
*/
/**
 * @brief   Tamanho máximo de uma string.
 */
#define SERIAL_FLEXCOMM_MAX_STRING_LENGTH   1024


//==============================================================================
//
// Tipos e estruturas de dados
//
//==============================================================================
/* DEV COMMENTS:
*		21/03/2019 - A estrutura canal serial foi criada junto com o Wagner fazer a adaptação do SDK da nxp com a biblioteca padrão
*
*/
/**
 * @brief   Abstração de um canal serial UART sobre um FlexCOMM
 */
typedef struct canal_serial
{
  //campos necessários à configuração do periférico FlexComm como UART
  //Ex: FlexComm_Addr, IRQ, Baudrate, etc.
  //Os campos abaixo precisam ser adaptados para usar a biblioteca de drivers do LPC
    USART_Type        *usart_base_addr;
	clock_name_t       flexcomm_clock;
	IRQn_Type          flexcomm_irq;       // FLEXCOMM0_IRQn, FLEXCOMM1_IRQn, ...
    usart_config_t     config;             // Estrutura de configuração da FlexCOMM
    ringbuf_t         *pInBuf;             // n muda Buffer de recepção, pendurado na INT de RX
    ringbuf_t         *pOutBuf;            // n muda Buffer de de saída (conteúdo do InBuf é trasferido para o Outbuf para poder ser processado)} hw_flexcomm_t;
} canal_serial_t;



//==============================================================================
//
// API
//
//==============================================================================
#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

//
// Configuração
//
bool serial_init(
    canal_serial_t *pSerial,
    ringbuf_t      *pInBuf,
    uint8_t        *pInBufArea,
    uint32_t        sizeInBuf,
    ringbuf_t      *pOutBuf,
    uint8_t        *pOutBufArea,
    uint32_t        sizeOutBuf,
    bool            useDefaultUartConfig
);
void serial_Clear(canal_serial_t *pSerial);
bool serial_SetBaudrate(canal_serial_t *pSerial, uint32_t baudrate_bps);
bool serial_SetParity(canal_serial_t *pSerial, usart_parity_mode_t parityMode);
bool serial_SetStopbit(canal_serial_t *pSerial, usart_stop_bit_count_t stopBitCount);

//
// Leitura
//
int serial_Available(canal_serial_t *pSerial);
uint32_t serial_Read(canal_serial_t *pSerial, uint32_t total_to_read);
uint32_t serial_ReadAll(canal_serial_t *pSerial);

//
// Escrita
//
void serial_Write(canal_serial_t *pSerial, uint8_t *pBuf, size_t buf_len);
void serial_WriteStr(canal_serial_t *pSerial, char *str);

//
// ISRs
//
void serial_FLEXCOMMn_IRQHandler(canal_serial_t *pSerial);

//
// Depuração
//
#if defined(LOGICALIS_DEBUG_SERIAL_FLEXCOMM) && (LOGICALIS_DEBUG_SERIAL_FLEXCOMM)

// Instância de configuração do canal serial usada na depuração
extern canal_serial_t debug_serial_instance;

// Rotina de teste e depuração desta lib
void debug_serial_echoServer();

#endif // LOGICALIS_DEBUG_SERIAL_FLEXCOMM


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // H_SERIAL_FLEXCOMM
