//==============================================================================
/**
\file    clock_lpc51u68.h
\brief   Configuração do sistema de clock da MCU LPC51U68.

\details
  Esta biblioteca implementa funções para configurar o clock do sistema
  com a QN9080.

  Para usar biblioteca esta biblioteca, é necessário definir nas propriedades
  do projeto uma macro chamada "LOGICALIS_CLOCK_LPC51U68", da seguinte
  forma:
    menu: Project
            |_ Properties
                |_ C/C++ General
                    |_ Paths and Symbosl
                        |_ Symbols
    macro:  LOGICALIS_CLOCK_LPC51U68
            valor:  1
            [x] Add to all configurations
            [x] Add to all languages
  isto permite que diferentes MCUs sejam suportadas em um mesmo firmwware.

*** IMPORTANTE ***

    1. Sobre este cabeçalho:
       Leia completamente todos os comentários deste cabeçalho para obter
       informações sobre o sistema de clock, RTC, VBAT, interrupção por
       falha do hardware e fontes de clock usadas pelos periféricos.

    2. Sobre RTC e VBAT:
       Se não houver tensão em VBAT, qualquer acesso aos registradores do
       RTC (RTC->registrador) resultará em uma interrupção por falha do
       hardware e ocorrerá um desvio para HardFault_Handler().
       Se a placa não possuir uma bateria para o RTC ou se VBAT não estiver
       conectado a VCC, então NÃO ATIVE O RTC

    3. Sobre as funções desta lib e o MCUXpresso Config Tools 3.0:
       As funções desta lib são modificações sobre as rotinas geradas pelo
       MCUXpresso Config Tools 3.0.
       Quando uma configuração é projetada no MCUXpresso Config Tools 3.0, o
       código referente a ela fica nas variáveis e nas rotinas do modo RUN,
       mesmo que a configuração seja para o modo HSRUN.
       No entanto, se o perfil for RUN, não é colocado o código para ajustar
       o modo de energia necessário ao modo RUN (no HSRUN isso é feito, mas
       não RUN).
       Por conta disso, as rotinas desta biblioteca não são um mero
       "copiar-e-colar" a partir do MCUXpresso Config Tools 3.0. Para criar
       o equivalente a estas rotinas para outra MCU, é necessário considerar
       todas as observações citadas ao longo deste cabeçalho.

	4. Testes iniciais mostraram que a USART não opera corretamente quando em
	   em 32KHz.

*** MCUXpresso Config Tools ***

    Sempre usá-lo para configurar o clock, pois além de ser um assistente
    gráfico, ele efetua consistências sobre a configuração e gera o código
    para ajustar o clock.
    IMPORTANTE: notei que a configuração feita no diagrama é refletida
                SEMPRE nas variáveis e nas rotinas referentes ao modo RUN,
                mesmo que esteja sendo configurado o modo HSRUN.

*** "LPC51U68 256KB Flash - User Guide"**

  https://www.mouser.com/pdfdocs/NXP_LPC51U68_UM.pdf



\b@{Histórico de Alterações:@}
	- 2019.05.07 -- Primeira versão (v1.0.0), baseada em clock_lpc51U68.h/.c

\author
  Renato Souza
\author
  Wagner A. P. Coimbra
*/
//==============================================================================
#define LOGICALIS_CLOCK_LPC51U68 1
#if defined(LOGICALIS_CLOCK_LPC51U68) && LOGICALIS_CLOCK_LPC51U68

#ifndef H_CLOCK_LPC51U68
#define H_CLOCK_LPC51U68

#include "board.h"
#include "fsl_power.h"

//==============================================================================
//
// Constantes
//
//==============================================================================

/**
 * @brief  Habilita (1) ou desabilita (0) a função de depuração desta lib
 */
#define LOGICALIS_DEBUG_CLOCK_LPC51U68          1

/**
 * @brief Frequências dos cristais na placa
 */
#define BOARD_BOOTCLOCKRUN_CORE_CLOCK                 32768U  /*!< Core clock frequency: 32768Hz */

/**
 * @brief Frequências geradas no modo 96MHz
 */
#define BOARD_BOOTCLOCKFROHF96M_CORE_CLOCK         96000000U  /*!< Core clock frequency: 96000000Hz */

/**
 * @brief Frequências geradas no modo 48MHz
 */
#define BOARD_BOOTCLOCKFROHF48M_CORE_CLOCK         48000000U  /*!< Core clock frequency: 48000000Hz */

/**
 * @brief Frequências geradas no modo 12MHz
 */
#define BOARD_BOOTCLOCKFRO12M_CORE_CLOCK           12000000U  /*!< Core clock frequency: 12000000Hz */

//==============================================================================
//
// Tipos e estruturas de dados
//
//==============================================================================

/**
 * @brief   Usada para configurar o método de wakeup do modo deep sleep
 */
typedef struct wake_from_deep_sleep_method
{
  //campos necessários à configuração do modo de acordar do deep sleep
  //Ex: GPIO, USART, etc..
    uint32_t        starter_mask;
} wake_from_deep_sleep_method_t;
//==============================================================================
//
// API
//
//==============================================================================

/**
 * @brief   Desliga o clock de 32768 do RTC.
 */
void clock_RCO32kPowerOff(void);

/**
 * @brief   Liga o clock de 32768 do RTC
 */
void clock_RCO32kPowerOn(void);

/**
 * @brief   Desliga o XTAL de 32K
 */
void clock_XTAL32KPowerOff(void);

/**
 * @brief   Liga a XTAL de 32K
 */
void clock_XTAL32KPowerOn(void);

/**
 * @brief   Desliga o OSC de 32M
 */
void clock_OSC32MPowerOff(void);

/**
 * @brief   Liga o OSC de 32M
 */
void clock_OSC32MPowerOn(void);

/**
 * @brief   Desliga o XTAL de 32M
 */
void clock_XTAL32MPowerOff(void);

/**
 * @brief   Liga XTAL de 32MHz
 */
void clock_XTAL32MPowerOn(void);

/**
 * @brief   Inicia configuração default de clock
 */
void clock_init(void);

/**
 * @brief   Altera clock para XTAL 8MHz
 */
void clock_modeXTAL8MHz(void);

/**
 * @brief   Altera clock para XTAL 16MHz
 */
void clock_modeXTAL16MHz(void);

/**
 * @brief  Altera clock para XTAL 32MHz
 */
void clock_modeXTAL32MHz(void);

/**
 * @brief   Altera clock para OSC 8MHz
 */
void clock_modeOSC8MHz(void);

/**
 * @brief   Altera clock para OSC 16MHz
 */
void clock_modeOSC16MHz(void);

/**
 * @brief   Altera clock para OSC 32MHz
 */
void clock_modeOSC32MHz(void);

/**
 * @brief   Altera clock para RCO 32KHz
 */
void clock_modeRCO32KHz(void);

/**
 * @brief   Altera clock para XTAL 32KHz
 */
void clock_modeXTAL32KHz(void);


/**
 * @brief   Ativa somente RTC
 *
 */
void clock_modeRtcOnly(void);

/**
 * @brief   Ativa modo sleep
 */
void clock_modeSleep(void);

/**
 * @brief   Ativa modo deep sleep e configura wake-up como sendo o SW1
 *
 */
void clock_modePowerDown(void);

//
// Depuração
//
#if defined(LOGICALIS_DEBUG_CLOCK_LPC51U68) && LOGICALIS_DEBUG_CLOCK_LPC51U68
void clock_debug(void);
void clock_debug_transitions(void);
#endif // LOGICALIS_DEBUG_CLOCK_LPC51U68

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // H_CLOCK_LPC51U68

#endif // LOGICALIS_CLOCK_LPC51U68
