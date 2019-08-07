// =============================================================================
/**
\file    mcu_watchdog.c
\brief   HAL do Watchdog da MCU.

\details
   Esta biblioteca implementa uma abstração sobre o hardware do Watchdog
   do sistema.

   No QN9080, O valor do Watchdog é reatualizado quando:
   	   -> O contador chega à zero.
   	   -> O gerador de interrupção é habilitado escrevendo no bit INTEN no
   	      Control Register CTRL, quando este estava desabilitado.
   	   -> Uma interrupção é limpada através do Interrupt Clear Register INT_CLR.
   	   -> Um novo valor é carregado no LOAD.

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
	- 2019.05.07 -- Primeira versão (v1.0.0)

\author
  Renato Zapata Lusni
\author
  Wagner A. P. Coimbra
*/
// =============================================================================
#include "fsl_wdt.h"
#include "mcu_watchdog.h"
#include "fsl_power.h"
#include "fsl_clock.h"

#include <stdio.h>

//==============================================================================
//
// API
//
//==============================================================================

/**
 * @brief  Liga o perifério WDOG.
 *
 *         O oscilador LPO (1kHz) é usado como fonte de clock do WDOG (conta milissegundos).
 *         Esta função pode ser chamada para reconfigurar o WDOG.
 *
 * @param  timeout_ms  Tempo aguardado até o WDOG resetar o sistema.
 *
 * @param  WDTClock    Valor do clock do WDT sendo utilizado.
 */
void watchdog_start(uint32_t timeout_ms,uint32_t WDTClock)
{

	// Iniciando estrutura de configuração
	wdt_config_t wdog_cfg;

	// Setando o valor do LOAD
	wdog_cfg.loadValue = (uint32_t)((timeout_ms*WDTClock)/1000);
	
	// Setando a variável de reset enable
	wdog_cfg.enableWdtReset = true;

    /* The WDT divides the input frequency into it by 4 */
    CLOCK_AttachClk(k32K_to_WDT_CLK);
    /* Check Reset source(First time device power on Watch dog reset source will be set) */
    if (kRESET_SrcWatchDog == RESET_GetResetSource())
    {
        printf("Watchdog reset occurred\r\n");
    }
    RESET_ClearResetSource();

    WDT_Init(WDT, &wdog_cfg);

    // Habilitando interrupção
    NVIC_EnableIRQ(WDT_IRQn);

    /* O watchdog só inicia após a operação de feed ser realizada
     * A operação foi incluida aqui para que o usuario não precise
     * se preocupar e seja apenas necessário iniciar o watchdog com
     * o tempo em milisegundos desejado e o valor do clock utilizado. */
    watchdog_refresh(timeout_ms,WDTClock);
}

/*!
 * @brief  Reinicia a contagem de tempo do WDOG.
 *         Execute esta função antes de estourar a contagem, do contrário ocorrerá um reset.
 */
void watchdog_refresh(uint32_t timeout_ms,uint32_t WDTClock)
{
	WDT_Refresh(WDT,(uint32_t)((timeout_ms*WDTClock)/1000));
}

/**
 * @brief  Desliga o perifério WDOG.
 *         Ao desligar o WDOG, o Reset Count é automaticamente zerado.
 */
void watchdog_stop()
{
	WDT_Deinit(WDT);
}

/**
 * @brief  Retorna o total de resets promovidos pelo WDOG.
 *
 * @return Total de resets causados por estouro do WDOG.
 */
uint16_t watchdog_getResetCount()
{
	//The TV register is used to read the current value of Watchdog timer counter.
	//When reading the value of the 24-bit counter, the lock and synchronization procedure
	//takes up to 6 WDCLK cycles plus 6 APB bus clock cycles, so the value of TV is older than
	//the actual value of the timer when it's being read by the CPU.
	// Diferente do MCU do ISD, neste LPC não há um contador de resets,
	// por este motivo, esta biblioteca retorna o valor 0 sempre.
	//return WWDT->TV;
	return 0;
}

/**
 * @brief  Limpa o contador de resets do WDOG.
 */
void watchdog_clearResetCount()
{
	//WDOG_ClearResetCount(WDOG);
	// Nao tem a funcao de cima no SDK do LPC
	// Talvez WWDT_Refresh(WWDT);
}



//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------

#if defined(ISD_DEBUG_MCU_WATCHDOG) && ISD_DEBUG_MCU_WATCHDOG

#include <stdio.h>
#include "delay.h"

/**
 * @brief  Rotina de teste desta biblioteca.
 *
 *         É gerado um relatório de funcionamento do WDOG, tal como este:
 *
 *           Iniciando testes com o WDOG
 *
 *
 *           ***** Teste 1: ocorrerao 3 resets, um a cada segundo
 *
 *           WDOG Reset Count: 0
 *           Fazendo delays 100ms...
 *           1.2.3.4.5.6.7.8.9.10.
 *
 *           WDOG Reset Count: 1
 *           Fazendo delays 100ms...
 *           1.2.3.4.5.6.7.8.9.10.
 *
 *           WDOG Reset Count: 2
 *           Fazendo delays 100ms...
 *           1.2.3.4.5.6.7.8.9.10.
 *
 *           ***** Teste 2: evitar resets, efetuando refresh
 *
 *           Fazendo delay de 5s, com refreshs a cada 0,5s:
 *           1.2.3.4.5.
 *           Nao resetou: Ok!
 *
 *           ***** Teste 3: limpar a contagem de resets
 *
 *           Contagem atual:        3
 *           Contagem apos o clear: 0
 *
 *           ***** Teste 4: stop no WDOG
 *
 *           Stop no WDOG: testando com delay de 5s SEM refreshs
 *           1.2.3.4.5.
 *           Nao resetou: Ok!
 *
 *           ***** Termino dos testes com o WDOG
 */

// incluir teste para checar relogio do watchdog
void debug_mcu_watchdog()
{
	uint16_t count = 0;
	printf("\n\n***** Testes 1 e 3 não são feitos pois o QN9080 não possui recurso de contagem de resets");
	//
	// Usando o LPO (1kHz) como fonte de clock, o WDOG resetará o sistema em intervalos de 1s
	//
	watchdog_start(500,32768);  //
	delay_ms(1);          // Isto aqui testa a reconfiguração do intervalo, sem fazer stop
	watchdog_refresh(1000,32768); //
	count = watchdog_getResetCount();

	//Testes 1 e 3 não são feitos pois o QN9080 não possui recurso de contagem de resets
	//
	// Título do TESTE 1
	//
//	if(count==0)
//	{
//		printf("\nIniciando testes com o WDOG\n");
//		printf("\n\n***** Teste 1: ocorrerao 3 resets, um a cada segundo");
//	}
//
//	//
//	// TESTE 1: exibirá esta mensagem 3 vezes
//	//
//	if(count < 3)
//	{
//		printf("\n\nWDOG Reset Count: %u", count);
//		printf("\nFazendo delays 100ms...\n");
//		for(int i=1; i<=50; i++)
//		{
//			printf("%d.", i);
//			delay_ms(100);
//		}
//	}

	//
	// TESTE 2: refresh sobre o WDOG
	//
	printf("\n\n***** Teste 2: evitar resets, efetuando refresh\n");
	printf("\nFazendo delay de 5s, com refreshs a cada 0,5s:\n");
	for(int i = 1; i<=5; i++)
	{
		printf("%d", i);
		delay_ms(500);
		watchdog_refresh(5000,32768);

		printf(".");
		delay_ms(500);
		watchdog_refresh(5000,32768);
	}
	printf("\nNao resetou: Ok!");

	//Testes 1 e 3 não são feitos pois o QN9080 não possui recurso de contagem de resets
	//
	// TESTE 3: limpar a contagem de resets
	//
//	printf("\n\n***** Teste 3: limpar a contagem de resets\n");
//	count = watchdog_getResetCount();
//	printf("\nContagem atual:        %u", count);
//	watchdog_clearResetCount();
//	count = watchdog_getResetCount();
//	printf("\nContagem apos o clear: %u", count);

	//
	// TESTE 4: stop no WDOG
	//
	printf("\n\n***** Teste 4: stop no WDOG\n");
	watchdog_stop();
	printf("\nStop no WDOG: testando com delay de 5s SEM refreshs\n");
	for(int i = 1; i<=5; i++)
	{
		printf("%d", i);
		delay_ms(500);

		printf(".");
		delay_ms(500);
	}
	printf("\nNao resetou: Ok!");

	printf("\n\n***** Termino dos testes com o WDOG");
}

#endif // ISD_DEBUG_MCU_WATCHDOG
