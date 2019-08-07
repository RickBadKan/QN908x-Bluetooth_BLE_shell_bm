// =============================================================================
/**
\file    log_uart.h
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
#ifndef H_LOG_UART
#define H_LOG_UART

#include <stdint.h>
#include <stdbool.h>
#include "fsl_clock.h"
#include "fsl_usart.h"

//------------------------------------------------------------------------------
//
// Constantes
//
//------------------------------------------------------------------------------

//
// Adicione nas propriedades do projeto (Item "paths and symbols") uma constante
// chamada:
//   LOGICALIS_LOG_UART_ACTIVE
//
// cujo valor poderá ser:
//   0 (suporte a depuração desativado)
//   1 (suporte a depuração ATIVADO)
//
// A variáveis abaixo devem ter valores que são suportados pelo SDK
// Nesta biblioteca não há tratamento de valores inválidos.
//

/**
 * @brief  Ativa (1) ou desativa (0) as rotinas de depuração desta lib.
 */
#define LOGICALIS_DEBUG_LOG_UART  1
/**
 * @brief   clock_name_t of the corresponding flexcomm.
 */
#define LOG_FLEXCOMM_CLOCK   kCLOCK_BusClk
/**
 * @brief   USART_Type utilizada como interface de log.
 */
#define LOG_USART_FLEXCOMM   USART0
/**
 * @brief   Baudrate em uint32_t da interface.
 */
#define LOG_USART_BAUD   115200U
/**
 * @brief   usart_stop_bit_count_t que representa a quantidade de stop bits.
 */
#define LOG_USART_STOP   kUSART_OneStopBit
/**
 * @brief   usart_parity_mode_t que corresponde à paridade.
 */
#define LOG_USART_PARITY   kUSART_ParityDisabled
/**
 * @brief   Tamanho do dado enviado pela USART.
 */
#define LOG_USART_DATA_LENGTH   kUSART_8BitsPerChar


//------------------------------------------------------------------------------
//
// API
//
//------------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus


#if defined (LOGICALIS_LOG_UART_ACTIVE) && (LOGICALIS_LOG_UART_ACTIVE)

//
// Configuração
//
void log_init();

//
// Escrita na UART
//
void log_writeBufAsHexString(uint8_t *pBuf, int16_t size);
void log_writeString(char *str);
void log_writeBool(bool value);
void log_write(uint8_t byte);
void log_writeBuf(uint8_t *pBuf, int16_t size);
void log_writeChar(char c);
void log_writeU32(uint32_t val);

//
// Apoio
//
char *byteToStringHEX(uint8_t byte);

#else // LOGICALIS_LOG_UART_ACTIVE



//------------------------------------------------------------------------------
//
// API "Nula"
//
//------------------------------------------------------------------------------

//
// Configuração
//
static inline void log_init() {}

//
// Escrita na UART
//
static inline void log_writeBufAsHexString(uint8_t *pBuf, int16_t size) {}
static inline void log_writeString(char *str) {}
static inline void log_writeBool(bool value) {}
static inline void log_write(uint8_t byte) {}
static inline void log_writeBuf(uint8_t *pBuf, int16_t size) {}
static inline void log_writeChar(char c) {}
static inline void log_writeU32(uint32_t val) {}

//
// Apoio
//
static inline char *byteToStringHEX(uint8_t byte) {return NULL;}

#endif // LOGICALIS_LOG_UART_ACTIVE



//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------
#if defined (LOGICALIS_DEBUG_LOG_UART) && (LOGICALIS_DEBUG_LOG_UART)
void debug_log_uart();
#endif // LOGICALIS_DEBUG_LOG_UART


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // H_LOG_UART
