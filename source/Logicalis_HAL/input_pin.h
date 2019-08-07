// =============================================================================
/**
\file    input_pin.h
\brief   Configuração de pinos como entrada digital, com ou sem suporte a
         interrupção.

\details
   Esta biblioteca abstrai a configuração de pinos GPIO que irão operar como
   entrada digital, seja com ou sem a detecção de mudança de estado por
   interrupções.

\b@{Histórico de Alterações:@}
    - 2017.09.15 - Criadas as funções:
                     inputPin_StateChanged()
                     inputPin_CurrentState()
                   (v1.0.1)
    - 2017.08.10 -- Primeira versão (v1.0.0)

\author
  Wagner A. P. Coimbra
*/
/* -----------------------------------------------------------------------------
Exemplo de uso:

	#include "board.h"
	#include "fsl_port.h"
	#include "fsl_gpio.h"
	#include "fsl_uart.h"
	#include "clock_config.h"
	#include "pin_mux.h"

	#include "delay.h"
	#include "rgb_led.h"
	#include "ring_buffer.h"
	#include "input_pin.h"

	//
	// Mapeamento de hardware do LED RGB
	//
	static hw_led_t hwLED_R = {
		.gpio = BOARD_LED_RED_GPIO,
		.pin =  BOARD_LED_RED_GPIO_PIN,
		.mask = BOARD_LED_RED_GPIO_MASK
	};

	static hw_led_t hwLED_G = {
		.gpio = BOARD_LED_GREEN_GPIO,
		.pin =  BOARD_LED_GREEN_GPIO_PIN,
		.mask = BOARD_LED_GREEN_GPIO_MASK
	};

	static hw_led_t hwLED_B = {
		.gpio = BOARD_LED_BLUE_GPIO,
		.pin =  BOARD_LED_BLUE_GPIO_PIN,
		.mask = BOARD_LED_BLUE_GPIO_MASK
	};

	//
	// Mapeamento de hardware do botão SW3
	//
	inputPin_t sw3_pin =
	{
	  .port          = PORTB,
	  .gpio          = GPIOB,
	  .pin           = PIN17_IDX,
	  .clock_periph  = kCLOCK_PortB,
	  .port_pull     = kPORT_PullUp,
	  .interrupt     = kPORT_InterruptEitherEdge,
	  .port_irq      = PORTB_IRQn,
	  .state_changed = false,
	  .current_state = 0
	};

	void PORTB_IRQHandler()
	{
      inputPin_IrqCheck(&sw3_pin);
	}

	void main()
	{
		// Configura periféricos da MCU
		BOARD_InitPins();
		BOARD_BootClockRUN();

		// Configura periféricos externos
		rgbInit(&hwLED_R, &hwLED_G, &hwLED_B);

		// Configura pinos de entrada
		inputPin_Init(&sw3_pin);

		rgbOff();
		while(1)
		{
			if( inputPin_StateChanged(&sw3_pin) )
				rgbBlue(!inputPin_CurrentState(&sw3_pin));
		}
	}

--------------------------------------------------------------------------------

COMO CRIAR BIBLIOTECAS E ADICIONÁ-LAS NA COMPILAÇÃO DO PROJETO

    Para criar uma pasta de biblioteca, clicar com botão direito no projeto e
    selecionar:
        New > Source Folder

    Para criar arquivos-fonte de biblioteca, clicar com o botão direito na
    pasta de biblioteca e selecionar:
        New > Source File
        New > Header File

	Para tornar as bibliotecas localizáveis com include "" e compiláveis,
	clicar com o botão direito no projeto e configurá-lo como a seguir:
		Properties
			C/C++ Build
			  Settings
				Aba "Tool Settings"
				  MCU Compiler
					Includes
					  Adicionar: "${workspace_loc:/${ProjName}/bibliotecas}"
								 Neste exemplo, a pasta se chama "bibliotecas"
				  MCU Assembler
					General
					  Adicionar: "${workspace_loc:/${ProjName}/bibliotecas}"
============================================================================= */
#ifndef H_INPUT_PIN
#define H_INPUT_PIN

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#include "fsl_inputmux.h"
#include "fsl_pint.h"
#include "fsl_gpio.h"
//------------------------------------------------------------------------------
//
// Tipos e estruturas de dados
//
//------------------------------------------------------------------------------

//
// Estrutura de configuração 
//
typedef struct _gpio_interrupt_config
{
	INPUTMUX_Type			*base;
	pint_pin_int_t			index;
	inputmux_connection_t	connection;
	PINT_Type               *pint;
	pint_pin_enable_t		enable;
	pint_cb_t				callback;
} gpio_interrupt_config_t;

//
// Definição de um pino de entrada GPIO.
// Permite habilitar interrupção.
//
typedef struct inputPin
{
	// Mapeamento de hardware do pino
	//(ORIGINAL)PORT_Type        *port;         // PORTA, PORTB, ...
	GPIO_Type         		 *gpio;         // GPIOA, GPIOB, ...
	uint32_t          		 port;
	uint32_t          		 pin;          // 0 to 31 (PIN0_IDX .. PIN31_IDX)
	clock_ip_name_t   		 clock_periph; // kCLOCK_PortA, kCLOCK_PortB, ...
	uint32_t          		 port_pull;    // kPORT_PullDisable, kPORT_PullDown or kPORT_PullUp (POSSO DEIXAR MAS NAO USA APARENTEMENTE)
	//(ORIGINAL)uint32_t           port_irq;     // PORTA_IRQn, PORTB_IRQn, ...

	// Configuração da interrupção
	gpio_interrupt_config_t *interrupt_config; // 0 = No Interrupt
	// Configuração de direção e estado inicial
	gpio_pin_config_t 		*config;
	
	// Estado de operação do pino
	bool               		state_changed; // Será true quando ocorrer uma interrupção
	uint32_t           		current_state; // Estado atual do pino
	uint32_t				port_pin_config;
} inputPin_t;



//------------------------------------------------------------------------------
//
// API
//
//------------------------------------------------------------------------------


void inputPin_Init(inputPin_t *input_pin_cfg);
void inputpin_PINT_Init(inputPin_t *inputPin_cfg);
void inputpin_enableinterrupt(inputPin_t *inputPin_cfg);
void inputPin_IrqCheck(inputPin_t *inputPin_cfg);
bool inputPin_StateChanged(inputPin_t *inputPin_cfg);
uint32_t inputPin_CurrentState(inputPin_t *inputPin_cfg);

//
// Depuração
//
#if defined(LOGICALIS_DEBUG_PIN_INPUT) && (LOGICALIS_DEBUG_PIN_INPUT)

// Instancias das estruturas para teste da biblioteca input_pin
extern gpio_interrupt_config_t interrupt_config;
extern gpio_pin_config_t gpio_config;
extern gpio_pin_config_t gpio_config;

// Rotina de teste e depuração desta lib
void debug_input_pin();

#endif // LOGICALIS_DEBUG_PIN_INPUT


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif //H_INPUT_PIN
