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
    - 2019.04.08 - Criadas primeira versão para LPC51U68:
    - 2017.09.15 - Criadas as funções:
                     inputPin_StateChanged()
                     inputPin_CurrentState()
                   (v1.0.1)
    - 2017.08.10 -- Primeira versão (v1.0.0)
\author
  Renato Souza
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
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_pint.h"
#include "fsl_iocon.h"

#include "input_pin.h"
//
// Configura o hardware do pino, tornando-o um pino GPIO de entrada.
//
void inputPin_Init(inputPin_t *inputPin_cfg)
{
	// Configura o pin_mux do pino
	IOCON_PinMuxSet(IOCON, inputPin_cfg->port, inputPin_cfg->pin, inputPin_cfg->port_pin_config);

	// Inicializa o estado atual da porta com 0.
	inputPin_cfg->current_state = inputPin_cfg->config->outputLogic;
	
	// Habilita o clock do GPIO passado dentro da config.
	CLOCK_EnableClock(inputPin_cfg->clock_periph);
	
	// BOARD_InitPins() não é feito nesta biblioteca - espera-se que o mux.c ja faça isso.
	
	
	// PinInit seta os seguintes registradores:
	//	-> DIR: Configura o pino como saida se 1, ou como entrada se 0.
	//	-> CLR: Usado para limpar o valor de saida do pino(independentemente do registrador MASK)
	// 	-> SET: Usado para setar o valor de saida do pino (independentemente do registrador MASK)
    GPIO_PinInit(inputPin_cfg->gpio, inputPin_cfg->pin, inputPin_cfg->config);

	// Escreve um valor na saida do pino, 0 escreve low_logic level e 1 escreve high_logic level
	// Checar se é saida antes
    if(inputPin_cfg->config->pinDirection)
    {
    	GPIO_WritePinOutput(inputPin_cfg->gpio, inputPin_cfg->pin, inputPin_cfg->config->outputLogic);
    }
		
	// Configuração das interrupções
	
	// Configuração de interrupção deve passar:
	// 	- enabled var indicating if interruption must be activated.
	//	- INPUTMUX_Type com o endereço da base do pint.
	//  - pint_pin_int_t com o index do PinInt.
	// 	- inputmux_connection_t com a conexão correspondente.
	//  - PINT_Type com o PINT correspondente.
	//  - pint_pin_enable_t com o tipo de interrupção (kPINT_PinIntEnableRiseEdge, kPINT_PinIntEnableFallEdge, etc...)
	//  - pint_cb_t que corresponde ao nome da função de callback.
	
	


}

void inputpin_PINT_Init(inputPin_t *inputPin_cfg)
{
	/* Initialize PINT */
	PINT_Init(inputPin_cfg->interrupt_config->pint);
}
void inputpin_enableinterrupt(inputPin_t *inputPin_cfg)
{
	// Checando se a interrupção deve ser habilitada ou não.
	if (inputPin_cfg->interrupt_config)
	{
		// Configuracao de interrupção:
		/* Connect trigger sources to PINT */
		INPUTMUX_Init(inputPin_cfg->interrupt_config->base);
		INPUTMUX_AttachSignal(inputPin_cfg->interrupt_config->base, inputPin_cfg->interrupt_config->index, inputPin_cfg->interrupt_config->connection);
		/* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
		INPUTMUX_Deinit(inputPin_cfg->interrupt_config->base);
		
	

		/* Setup Pin Interrupt 0 for rising edge */
		PINT_PinInterruptConfig(inputPin_cfg->interrupt_config->pint,
								inputPin_cfg->interrupt_config->index,
								inputPin_cfg->interrupt_config->enable,
								inputPin_cfg->interrupt_config->callback);
		/* Enable callbacks for PINT */
		PINT_EnableCallback(inputPin_cfg->interrupt_config->pint);
	}
}
//
// IMPORTANTE: utilize esta rotina dentro da IRQ do PORT. Ela
//             trata as flags de interrupção do pino.
//
// Verifica a interrupção ocorrida e atualiza os campos internos de
// state_changed e current_state.
//
void inputPin_IrqCheck(inputPin_t *inputPin_cfg)
{
	// Máscara de bit do pino.
	//uint32_t pin_mask = 1U << inputPin_cfg->pin;

	// Lê a flag de interrupção.
	//uint32_t pin_intflag = pin_mask & PINT_PinInterruptGetStatus(inputPin_cfg->interrupt_config->pint, inputPin_cfg->interrupt_config->index);

	// Limpa flags de interrupção do pino.
    PINT_PinInterruptClrStatus(inputPin_cfg->interrupt_config->pint, inputPin_cfg->interrupt_config->index);

	// Verifica se ocorreu interrupção no pino.
	//if(pin_intflag)
	//{
	inputPin_cfg->state_changed = true;
	inputPin_cfg->current_state = GPIO_ReadPinInput(inputPin_cfg->gpio, inputPin_cfg->pin);
	//}
}

//
// Retorna true se tiver ocorrido uma alteração no estado do pino
//
bool inputPin_StateChanged(inputPin_t *inputPin_cfg)
{
	return inputPin_cfg->state_changed;
}

//
// Retorna o estado atual do pino
//
uint32_t inputPin_CurrentState(inputPin_t *inputPin_cfg)
{
	inputPin_cfg->state_changed = false;
	return inputPin_cfg->current_state;
}
//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------

#if defined(LOGICALIS_DEBUG_PIN_INPUT) && (LOGICALIS_DEBUG_PIN_INPUT)
// Como usar: Criar uma callback no source do projeto e passar a callback na configuração.
// A callback deve possuir a seguinte estrutura:
//void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
//{
//}

#include <stdio.h>

// Inicia configuração do switch

// Define a estrtura que configura interrupção
gpio_interrupt_config_t interrupt_config = {
	.enable = 1;
	.INPUTMUX_Type = INPUTMUX,
	.pint_pin_int_t = kPINT_PinInt0,
	.inputmux_connection_t = kINPUTMUX_GpioPort0Pin24ToPintsel,
	.pint = PINT,
	.pint_pin_enable_t = kPINT_PinIntEnableFallEdge,
	.pint_cb_t = pint_intr_callback
};

/* Define the init structure for the output LED pin*/
gpio_pin_config_t gpio_config = {
    kGPIO_DigitalOutput, 0,
};
inputPin_t inputPin_Init = {
	.port = 0,
	.gpio = GPIO,
	.pin = 24
	.clock_periph = kCLOCK_Gpio0,
	.port_pull = 0, // nao usado
	.interrupt_config = &interrupt_config
	.config = gpio_config,
	.state_changed = 0, // nao alterar na configuração
	.current_state = 0 // nao alterar na configuração
};


// Inicia configuração do led

void debug_input_pin()
{
	// Iniciando o input pin do switch
	inputPin_Init(inputPin_Init);
	
	// Iniciando o input pin do led
	// inputPin_Init(inputPin_Init_led);
}

#endif // LOGICALIS_DEBUG_PIN_INPUT
