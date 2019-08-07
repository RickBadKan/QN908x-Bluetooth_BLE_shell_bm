// =============================================================================
/**
\file    mcu_watchdog.h
\brief   HAL do Watchdog da MCU.

\details
   Esta biblioteca implementa uma abstração sobre o hardware do Watchdog
   do sistema.

   IMPORTANTE:
     1. Para habilitar a função de apoio a depurações:
          debug_mcu_watchdog()
        descomente a macro:
          #define ISD_DEBUG_MCU_WATCHDOG 1

     2. Sobre funções "static inline" e erros de compilação
        Quando uma função é declarada como "inline", ela é automaticamente
        assumida como "static" (na gramática do GCC, é obrigatório, inclusive,
        colocar o operador de armazenamento "static" antes do "inline").

        "inline" significa que é feita uma cópia do corpo da função para cada
        local em que ela é chamada (isso consome mais espaço, mas aumenta o
        desempenho).

        "static" significa que a função só é visível no módulo (arquivo .c)
        em que ela é declarada e usada.

        O problema da "static inline" reside na forma como o compilador entende
        a declaração e o uso dessas funções e isso leva a dois requisitos:
          - Se as funções "static inline" forem públicas, elas precisam ser
            declaradas e implementadas nos arquivos .h (se estiverem no .c,
            elas deixam de ser visíveis para outros módulos).
          - o #include para elas ser colocado direamente no arquivo .c que as
            utiliza. Se for colocado no .h, ocorrerá o erro:
              "invalid storage class function"
            Por exemplo, se o:
              #include <fsl_wdog.h>
            do arquivo .c desta biblioteca for movido para o .h , ocorrerá este
            erro em cada uma das funções "static inline" de "fsl_wdog.h".

\b@{Histórico de Alterações:@}
	- 2018.12.08 -- Primeira versão (v1.0.0)

\author
  Wagner A. P. Coimbra
*/
// =============================================================================
#ifndef H_MCU_WATCHDOG
#define H_MCU_WATCHDOG

#if defined(__cplusplus)
extern "C"
{
#endif // __cplusplus



//==============================================================================
//
// Constantes e estruturas de dados
//
//==============================================================================

/*!
 * Habilita a função de apoio a depurações debug_mcu_watchdog()
 */
#define ISD_DEBUG_MCU_WATCHDOG  1

#define MCU_WATCHDOG_WINDOW 0xFFFFFF // 0xFFFFFF - no window
#define MCU_WATCHDOG_WARNING 0 //0 - no warning
#define MCU_WATCHDOG_ENABLE_RESET true
//==============================================================================
//
// API
//
//==============================================================================

void watchdog_start(uint32_t timeout_ms,uint32_t WDTClock);
void watchdog_refresh(uint32_t timeout_ms,uint32_t WDTClock);
void watchdog_stop();
uint16_t watchdog_getResetCount();
void watchdog_clearResetCount();

//
// Depuração
//
#if defined(ISD_DEBUG_MCU_WATCHDOG) && ISD_DEBUG_MCU_WATCHDOG
void debug_mcu_watchdog();
#endif // ISD_DEBUG_MCU_WATCHDOG



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif  // H_MCU_WATCHDOG
