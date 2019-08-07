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
#include "clock_QN9080.h"
#if defined(LOGICALIS_CLOCK_LPC51U68) && LOGICALIS_CLOCK_LPC51U68
#include "fsl_power.h"
#include "fsl_clock.h"
#include "delay.h"
#include "log_uart.h"
#include "fsl_calibration.h"
#include "fsl_pint.h"
#include "fsl_iocon.h"
#include "QN908XC.h"

//==============================================================================
//
// Variáveis Externas
//
//==============================================================================

//
// System clock frequency
//
extern uint32_t SystemCoreClock;

//==============================================================================
//
// Funções Estáticas
//
//==============================================================================

/**
 * @brief   Faz as configurações iniciais e ativa 12MHz como main_clock
 */
static void clock_default(void)
{
	// Calibração do sistema
	CALIB_SystemCalib();

	/*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  	/*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  	/*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);                 	 	/*!< Switch SYS_CLK to XTAL */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  		/*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);               /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 0U);                 /*!< Set XTAL_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);                  /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  	/*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  	/*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/
    SystemCoreClock = CLK_XTAL_32MHZ;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 12MHz
 */
static void clock_XTAL32MHzOn(void)
{
    /*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  	/*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                   /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);                  	/*!< Switch SYS_CLK to XTAL */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  		/*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);               /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 0U);                 /*!< Set XTAL_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);                  /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/
    SystemCoreClock = CLK_XTAL_32MHZ;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 48MHz
 */
static void clock_XTAL16MHzOn(void)
{
    /*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);                  /*!< Switch SYS_CLK to XTAL */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);                  /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 1U);                  /*!< Set XTAL_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);                  /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/
    SystemCoreClock = CLK_XTAL_16MHZ;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 96MHz
 */
static void clock_XTAL8MHzOn(void)
{

    /*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);                  /*!< Switch SYS_CLK to XTAL */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);                  /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 1U);                  /*!< Set XTAL_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);                  /*!< Set AHB_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/
    SystemCoreClock = 8000000U;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 96MHz
 */
static void clock_OSC32MHzOn(void)
{
	// Turn OSC32MHz on
	clock_OSC32MPowerOn();

	/*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kOSC32M_to_SYS_CLK);                  /*!< Switch SYS_CLK to OSC32M */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);                  /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 0U);                  /*!< Set XTAL_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);                  /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/

    SystemCoreClock = CLK_OSC_32MHZ;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 96MHz
 */
static void clock_OSC16MHzOn(void)
{
	// Turn OSC32MHz on
	clock_OSC32MPowerOn();

	/*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kOSC32M_to_SYS_CLK);                  /*!< Switch SYS_CLK to OSC32M */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 0U);                  /*!< Set OSC32M_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 0U);                  /*!< Set XTAL_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);                  /*!< Set AHB_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/

    SystemCoreClock = 16000000U;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 96MHz
 */
static void clock_OSC8MHzOn(void)
{
	// Turn OSC32MHz on
	clock_OSC32MPowerOn();

	/*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);                  /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kOSC32M_to_SYS_CLK);                  /*!< Switch SYS_CLK to OSC32M */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 1U);                  /*!< Set OSC32M_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 0U);                  /*!< Set XTAL_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);                  /*!< Set AHB_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);                  /*!< Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);                  /*!< Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/

	SystemCoreClock = 8000000U;
}

/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 32KHz
 */
static void clock_XTAL32kOn(void)
{
	/*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                 /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);              /*!< Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(k32K_to_SYS_CLK);                  /*!< Switch SYS_CLK to 32K */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);             /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);             /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/
    SystemCoreClock = CLK_XTAL_32KHZ;
}
/**
 * @brief   Altera a configuraçãom de clock e muda o main_clk para 32KHz
 */
static void clock_RCO32kOn(void)
{
    /*!< Power up/Power down the module. */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);                  /*!< Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(kRCO32K_to_32K_CLK);                  /*!< Switch 32K_CLK to RCO32K */
    CLOCK_AttachClk(k32K_to_SYS_CLK);                  /*!< Switch SYS_CLK to 32K */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);                  /*!< Switch WDT_CLK to APB */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 0U);                  /*!< Set AHB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);                  /*!< Set APB_DIV divider to value 1 */

    /*!< Enable/Disable clock out source and pins.*/

    /*!< Enable/Disable the specified peripheral clock.*/

    SystemCoreClock = CLK_RCO_32KHZ;
}
//==============================================================================
//
// API
//
//==============================================================================

/**
 * @brief   Desliga o clock de 32768 do RTC.
 */
void clock_RCO32kPowerOff(void)
{
    /*!< Power up/Power down the module. */
    POWER_EnablePD(kPDRUNCFG_PD_RCO32K);       /*!< Power down XTAL32K */
}

/**
 * @brief   Liga o clock de 32768 do RTC
 */
void clock_RCO32kPowerOn(void)
{
	/*!< Power up/Power down the module. */
    POWER_DisablePD(kPDRUNCFG_PD_RCO32K);       /*!< Power down XTAL32K */
}

/**
 * @brief   Desliga o XTAL de 32K
 */
void clock_XTAL32KPowerOff(void)
{
    /*!< Power up/Power down the module. */
    POWER_EnablePD(kPDRUNCFG_PD_XTAL32K);       /*!< Power down XTAL */
}

/**
 * @brief   Liga a XTAL de 32K
 */
void clock_XTAL32KPowerOn(void)
{
    /*!< Power up/Power down the module. */
    POWER_DisablePD(kPDRUNCFG_PD_XTAL32K);       /*!< Power down XTAL */
}

/**
 * @brief   Desliga o OSC de 32M
 */
void clock_OSC32MPowerOff(void)
{
	/*!< Power up/Power down the module. */
	POWER_EnablePD(kPDRUNCFG_PD_OSC32M);
}

/**
 * @brief   Liga o OSC de 32M
 */
void clock_OSC32MPowerOn(void)
{
	/*!< Power up/Power down the module. */
	POWER_DisablePD(kPDRUNCFG_PD_OSC32M);
}

/**
 * @brief   Desliga o XTAL de 32M
 */
void clock_XTAL32MPowerOff(void)
{
    /*!< Power up/Power down the module. */
    POWER_EnablePD(kPDRUNCFG_PD_XTAL);       /*!< Power down XTAL */
}

/**
 * @brief   Liga XTAL de 32MHz
 */
void clock_XTAL32MPowerOn(void)
{
   /*!< Power up/Power down the module. */
    POWER_DisablePD(kPDRUNCFG_PD_XTAL);       /*!< Power down XTAL */


}

/**
 * @brief   Inicia configuração default de clock
 */
void clock_init(void)
{
	// Inicia configuração default de clock
	clock_default();
}

/**
 * @brief   Altera clock para XTAL 8MHz
 */
void clock_modeXTAL8MHz(void)
{
	clock_XTAL8MHzOn();
}

/**
 * @brief   Altera clock para XTAL 16MHz
 */
void clock_modeXTAL16MHz(void)
{
	clock_XTAL16MHzOn();
}

/**
 * @brief  Altera clock para XTAL 32MHz
 */
void clock_modeXTAL32MHz(void)
{
	clock_XTAL32MHzOn();
}

/**
 * @brief   Altera clock para OSC 8MHz
 */
void clock_modeOSC8MHz(void)
{
	clock_OSC8MHzOn();
}

/**
 * @brief   Altera clock para OSC 16MHz
 */
void clock_modeOSC16MHz(void)
{
	clock_OSC16MHzOn();
}

/**
 * @brief   Altera clock para OSC 32MHz
 */
void clock_modeOSC32MHz(void)
{
	clock_OSC32MHzOn();
}

/**
 * @brief   Altera clock para RCO 32KHz
 */
void clock_modeRCO32KHz(void)
{
	clock_RCO32kOn();
}

/**
 * @brief   Altera clock para XTAL 32KHz
 */
void clock_modeXTAL32KHz(void)
{
	clock_XTAL32kOn();
}


/**
 * @brief   Ativa somente RTC
 *
 */
void clock_modeRtcOnly(void)
{
	// TODO: Ainda não está feito pos é necessário analisar a parte do RTC
}

/**
 * @brief   Ativa modo sleep
 */
void clock_modeSleep(void)
{
	// Não utilizado
}

/**
 * @brief   Ativa modo deep sleep e configura wake-up como sendo o SW1
 *
 */
void clock_modePowerDown(void)
{
	
	/* Desabilitando as interrupções ativas do PINT ativo, para não
	 * 	acordar o módulo - Comportamento também presente no exemplo
	 * 	do SDK
	 */
	PINT_DisableCallback(PINT);
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);

	__disable_irq();

	NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
	NVIC_EnableIRQ(EXT_GPIO_WAKEUP_IRQn);


	/* Enable gpio wakeup at button2 */
	SYSCON->PIO_WAKEUP_LVL0 = SYSCON->PIO_WAKEUP_LVL0 | BOARD_SW1_GPIO_PIN_MASK;
	SYSCON->PIO_WAKEUP_EN0 = SYSCON->PIO_WAKEUP_EN0 | BOARD_SW1_GPIO_PIN_MASK;
    // Enable WAKEUP_EN bit in PMU_CTRL0 register
	SYSCON->PMU_CTRL0 |= SYSCON_PMU_CTRL0_PMU_EN_MASK;



	/* Enable GPIO wakeup */
	POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(0U));
	SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(0U);


	/*
	 * FROM QN9080 Userguide:
	 * 	In active mode, I/O status is defined by user configuration. While before entering
	 * 	power-down mode, user need to capture the I/O status, so that it will hold in power-down
	 * 	mode. The flow is:
	 * 		• Write 1 to IO_CAP register described in Section 2, to capture the I/O status
	 * 		• Write PDM_IO_SEL bit to 1 in PIO_WAKEUP_EN1 register described in Section 2.
	 * 		  After that, I/O status is decided by the value captured in the registers
	 * 		• Enter WFI to sleep
	 * 		• Wakeup, and write PDM_IO_SEL bit to 0 in PIO_WAKEUP_EN1 register to recover
	 * 		  the IO status from power-down mode. After that, I/O status is user configurable
	 */
	POWER_LatchIO();
	CLOCK_DisableClock(kCLOCK_Flexcomm0);
	POWER_EnterPowerDown(0);
	CLOCK_EnableClock(kCLOCK_Flexcomm0);
	POWER_RestoreIO();


	 /* switch to XTAL after it is stable */
	while (!(SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK & SYSCON->SYS_MODE_CTRL));
	SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(1U);
	POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(1U));


	/* Reativando as interrupções de PINT */

	// Reativando interrupções do NVIC
	NVIC_DisableIRQ(EXT_GPIO_WAKEUP_IRQn);
	NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);

	__enable_irq();

	// Reativando interrupções do Systick
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);

	// Reativando interrupções do PINT
	PINT_EnableCallback(PINT);

}

//==============================================================================
//
// Depuração
//
//==============================================================================

#if defined(LOGICALIS_DEBUG_CLOCK_LPC51U68) && LOGICALIS_DEBUG_CLOCK_LPC51U68

#include <stdio.h>

/**
 * @brief   Rotina de depuração para qualquer clock exceto 32768.
 *
 */
static void estimate_clock()
{
	// Frequência de interrupção do systick
	uint32_t sysTickFreq = 1000;
	/* Esta rotina requer a declaração de interrupção do systick presente no main. */
	/* Initialize log_uart */
    log_init();

    /* Initialize delay init */
    delay_Init(sysTickFreq);

    /* Initialize count reference milibefore, milinow, and milidiff*/
    uint32_t milibefore=time_now_tick();
    uint32_t milinow=0U;
    uint32_t milidiff=0U;

    /* Para melhorar a estimativa é descontado a quantidade de ciclos
     * a mais que a rotina delay_cycles32 leva além da quantidade de
     * ciclos passada como argumento.
     * A conta a ser feita para calcular o atraso:
     *
     * 		(|milidiff - (10000000/(SystemCoreClock/1000))|*SystemCoreClock/1000)*1000
     *
     * 		Onde 1000 é configurado hardcoded no systick.
     *
     * 		O fator resultante é uma média dos fatores nos 3 diferentes clocks,
     */

    /* Espera 10 milhões de ciclos */
    delay_cycles32(10000000-1020000);


    milinow=time_now_tick();
    milidiff=time_diff_tick(milibefore,milinow);

    /* Envia a diferença na contagem */
    log_writeString("10.000.000 cycles corresponds to: ");
    log_writeU32(milidiff);
    log_writeString(" interruptions.");
    log_write(13); //return
    log_write(10); //new line

    /* Calcula e envia a estimativa de clock */
    uint32_t clock_estimated = (10000000/milidiff)*1000;
    log_writeString("estimated clock: ");
    log_writeU32(clock_estimated);
    log_write(13); //return
    log_write(10); //new line
    log_writeString("system core clock: ");
    log_writeU32(SystemCoreClock);
    log_write(13); //return
    log_write(10); //new line
}
/**
 * @brief   Rotina de depuração para o clock de 32768 ou 32000
 */
static void estimate_clock_32()
{
	// Frequência de interrupção do systick
	uint32_t sysTickFreq = 100;
	/* Esta rotina requer a declaração de interrupção do systick presente no main. */


    /* Initialize delay init */
    delay_Init(sysTickFreq);

    /* Initialize count reference milibefore, milinow, and milidiff*/
    uint32_t milinow=0U;
    uint32_t milidiff=0U;
    uint32_t milibefore; // milibefore é definido aqui e usado abaixo para economizar ciclos.
    milibefore=time_now_tick();


    /* Para melhorar a estimativa é descontado a quantidade de ciclos
     * a mais que a rotina delay_cycles32 leva além da quantidade de
     * ciclos passada como argumento.
     * A conta a ser feita para calcular o atraso:
     *
     * 		(|milidiff - (10000000/(SystemCoreClock/sysTickFreq))|*SystemCoreClock/1000)*1000
     *
     * 		Onde sysTickFreq é configurado de freq iniciada no systick.
     *
     * 		O fator resultante é uma média dos fatores nos 3 diferentes clocks,
     */

    /* Espera 100mil de ciclos
     *  28500 para que a contagem entre milibefore e milinow seja a mais proxima possivel
     *  de 100000. */
    delay_cycles32(100000-28500);

    milinow=time_now_tick();
    milidiff=time_diff_tick(milibefore,milinow);

    /* Na frequencia de 32KHz o log_uart não funciona */

    /* Envia a diferença na contagem */
    printf("100.000 cycles corresponds to: %d interruptions\n",milidiff);

    /* Calcula e envia a estimativa de clock */
    uint32_t clock_estimated = (100000*100)/milidiff;


    printf("Estimated clock: %d\n",clock_estimated);


    printf("System core clock: %d\n",SystemCoreClock);


}

/**
 * @brief   Rotina de depuração da biblioteca de clock.
 */
void clock_debug(void)
{
	if (SystemCoreClock>=8000000)
	{
		estimate_clock();
	}
	else
	{
		estimate_clock_32();
	}
}

/**
 * @brief   Rotina de depuração das transições de clock.
 */
void clock_debug_transitions(void)
{
	/* Initialize log_uart */
    log_init();
    log_writeString("\n\rIniciando debug de transições\n\r");

    // Teste todas as possíveis transições de clock - Clocks baixos não conseguem utilizar o log_uart.
    clock_XTAL32MHzOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL32MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_XTAL32MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL16MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_XTAL16MHzOn();
    clock_debug();

    clock_XTAL8MHzOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();

    clock_XTAL8MHzOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();

    clock_XTAL8MHzOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();

    clock_XTAL8MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();

    clock_XTAL8MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_XTAL8MHzOn();
    clock_debug();

    clock_OSC32MHzOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();

    clock_OSC32MHzOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();

    clock_OSC32MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();

    clock_OSC32MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_OSC32MHzOn();
    clock_debug();

    clock_OSC16MHzOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();

    clock_OSC16MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();

    clock_OSC16MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_OSC16MHzOn();
    clock_debug();

    clock_OSC8MHzOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();

    clock_OSC8MHzOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_OSC8MHzOn();
    clock_debug();

    clock_XTAL32kOn();
    clock_debug();
    clock_RCO32kOn();
    clock_debug();
    clock_XTAL32kOn();
    clock_debug();

    /* Going back to 16MHz */
    clock_XTAL16MHzOn();

}

#endif // LOGICALIS_DEBUG_CLOCK_LPC51U68
#endif // LOGICALIS_CLOCK_LPC51U68
