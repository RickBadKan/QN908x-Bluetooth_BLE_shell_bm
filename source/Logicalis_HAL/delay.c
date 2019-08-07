/*******************************************************************************
delay.c

Rotinas de contagem de tempo, baseadas em NOP e no Systick.

Autor: Wagner A. P. Coimbra

--------------------------------------------------------------------------------

Resumo:

  MCU         : MK22FN256xxx12
  Versão      : 2017.10.04
  Compilador  : GNU ARM C Cross-Compiler (da toolchain "NXP MCU Tools")
  Conteúdo    : API:
                  delay_Init()
                  random_Init()
                  delay_10Cycles()   <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_100Cycles()  <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_cycles()     <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_cycles32()   <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_ns()         <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_us()         <- depende do Core Clock, que não pode mudar ao longo do programa
                  delay_ms()         <- depende do Core Clock, que não pode mudar ao longo do programa
                  time_now_ms()      <- depende de SysTick (não é afetado pelo Core Clock)
                  time_diff_ms()
                ISR's:
                  SysTick_Handler()
  Dependências:
                - A variável "SystemCoreClock" precisa estar atualizada com o
                  Core Clock real
                - Usa o SysTick em time_now_ms().
                  Portanto, é necessário implementar: void SysTick_Handler()

--------------------------------------------------------------------------------

Histórico de atualizações:

2017.10.04 - Modificada para suportar delays de ciclos de clock, ns e us, com
             base na biblioteca WAIT1.h/.c da "MCU on Eclipse"
2017.09.08 - Criadas funções random_Init(), time_now_ms() e time_diff_ms()
2017.08.31 - Criada versão 1.0

--------------------------------------------------------------------------------

LINT

  Lint é uma ferramenta de apoio a depurações, que pode ser usada, entre outras
  coisas, para ativar ou desativar a apresentação de mensagens.

  Na biblioteca delay.h/.c, o lint foi usado com os seguintes argumentos:
    -save
        Salva o contexto de depuração (níveis de warning, warnings suprimidos, etc).
    -restore
        Restaura o contexto de depuração, salvo previamente.
    -e(valor1, ..., valorN)
        Não apresenta (exclui) as mensagens identificadas pelos valores informados.
        No Lint, as mensagens de depuração foram mapeadas da seguinte forma:

                                  C               C++         Warning Level
          Syntax Errors         1 - 199      1001 - 1199            1
          Internal Errors     200 - 299                             0
          Fatal Errors        300 - 399                             0
          Warnings            400 - 699      1400 - 1699            2
          Informational       700 - 899      1700 - 1899            3
          Elective Notes      900 - 999      1900 - 1999            4

  Também na biblioteca delay.h/.c, o argumento -e() foi usado para suprimir as
  seguintes mensagens de advertência:
    505   Redundant left argument to comma
            The left argument to the comma operator had no side effects in its
            top-most operator and hence is redundant.
    506   Constant value Boolean
            A Boolean, i.e., a quantity found in a context that requires a
            Boolean such as an argument to && or || or an if() or while() clause
            or ! was found to be a constant and hence will evaluate the same
            way each time.
    522   Expected void type, assignment, increment or decrement.
            If a statement consists only of an expression, it
            should either be an expression yielding the void type or
            be one of the privileged operators: assignment, increment,
            or decrement.  Note that the statement *p++; draws this
            message but p++; does not.  This message is frequently
            given in cases where a function is called through a
            pointer and the return value is not void.  In this case we
            recommend a cast to void.  If your compiler does not
            support the void type then you should use the -fvo option.

  Referência:
    http://www.gimpel.com/html/pub/msg.txt
    04/10/2017

--------------------------------------------------------------------------------

__attribute__ - Atributos de funções

  Os atributos descritos aqui, bem como a forma como foram declarados e usados,
  é algo específico ao GCC.

  * no_instrument_function
      "Instrumentation" é um dentre os métodos para coleta de dados de
      desempenho (tracing), que gera informações detalhadas sobre o tempo
      de execução de uma função, toda vez que essa função é chamada.

      Nas rotinas que efetuam delay da ordem de us e de ns, este atributo
      foi desabilitado para não degradar a contagem de tempo nessas funções.

      Referências:
        "Understanding Performance Collection Methods"
        https://msdn.microsoft.com/en-us/library/dd264994.aspx
        03/10/2017

  * naked
      Este atributo indica que a função não precisa que o compilador gere
      sequências de "prólogo" e de "epílogo" no início e no final da função,
      respectivamente.

      A sequência de prólogo prepara registradores e a pilha para salvar o
      contexto e a de epílogo restaura o contexto previamente salvo.

      Somente funções com instruções em Assembly sem argumentos podem receber
	  o atributo naked (tal como as rotinas de delay da ordem de ns e us).

  Referências:
    "5.27 Declaring Attributes of Functions"
    https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Function-Attributes.html
    03/10/2017

    "Function prologue"
    https://en.wikipedia.org/wiki/Function_prologue
    03/10/2017
*******************************************************************************/
#include <stdlib.h>
#include "fsl_common.h"
#include "delay.h"
#include "log_uart.h"


//------------------------------------------------------------------------------
//
// Variáveis globais
//
//------------------------------------------------------------------------------

//
// Contador de milissegundos, incrementado pelo SysTick.
//
volatile uint32_t g_current_ms_count = 0u;

//
// Valor do divisor para realizar a conversão de ticks para ms.
//
volatile uint32_t divisor = 0u;

//
// Valor do multiplicador para realizar a conversão de ticks para ns.
//
volatile uint32_t multiplicador = 0u;

// Já tinha essa parte no código enviado pelo Wagner
//
// Contador de milissegundos a esperar, decrementado em delay_ms()
//
//volatile uint32_t s_milliseconds_left = 0u;



//------------------------------------------------------------------------------
//
// API
//
//------------------------------------------------------------------------------

/**
 * @brief   Inicia as configurações para incia as interrupções do systick.
 *
 * @param 	freq consiste na frequência em que o systick irá interrupções.
 *
 * @detail o Systick é utilizado nas funções:
 *	 - delay_ms()
 *	 - time_now_ms()
 */
void delay_Init(uint32_t freq)
{
	// Definindo os valores de divisor e multiplicador
	// casos:
	//		freq = 100000   T = 1/100000
	//      contagem 139289   treal = 139289/100000   -> Para corrigir multiplica por sem, ou freq/1000
	//
	//		freq = 100 		T = 1/100
	//		contagem 139289   treal = 139289/100	  -> Para corrigir divide por dez, ou 1000/freq
	if(freq >= 1000)
	{
		divisor = freq;
		multiplicador = 1000;
	}
	else
	{
		divisor = freq;
		multiplicador = 1000;
	}

	// Programa o SysTick para interromper na frequência freq.
	SysTick_Config(SystemCoreClock/freq);
}

/**
 * @brief   Efetua uma espera de 10 ciclos do CORE CLOCK, usando NOP.
 *
 */
__attribute__((naked, no_instrument_function)) void delay_10cycles()
{
  // This function will wait 10 CPU cycles (including call overhead).
  // NOTE: Cortex-M0 and M4 have 1 cycle for a NOP
  //       Compiler is GNUC

  /*lint -save -e522 function lacks side effect. */
  __asm (
   /* bl Wai10Cycles() to here: [4] */
   "nop   \n\t" /* [1] */
   "nop   \n\t" /* [1] */
   "nop   \n\t" /* [1] */
   "bx lr \n\t" /* [3] */
  );
  /*lint -restore */
}

/**
 * @brief   Efetua uma espera de 100 ciclos do CORE CLOCK, usando NOP.
 *
 */
__attribute__((naked, no_instrument_function)) void delay_100cycles()
{
  // This function will spend 100 CPU cycles (including call overhead).
  // NOTE: Cortex-M0 and M4 have 1 cycle for a NOP
  //       Compiler is GNUC

  /*lint -save -e522 function lacks side effect. */
  __asm (
   /* bl to here:               [4] */
   "movs r0, #0 \n\t"        /* [1] */
   "loop:       \n\t"
   "nop         \n\t"        /* [1] */
   "nop         \n\t"        /* [1] */
   "nop         \n\t"        /* [1] */
   "nop         \n\t"        /* [1] */
   "nop         \n\t"        /* [1] */
   "add r0,#1   \n\t"        /* [1] */
   "cmp r0,#9   \n\t"        /* [1] */
   "bls loop    \n\t"        /* [3] taken, [1] not taken */
   "nop         \n\t"        /* [1] */
   "bx lr       \n\t"        /* [3] */
  );
  /*lint -restore */
}

/**
 * @brief   Efetua uma espera de N ciclos do CORE CLOCK, usando NOP.
 *          N é um número inteiro de 16 bits, maior ou igual a 10.
 *
 * @param   Cycles é valor correspondente à quantidade de ciclos que
 * 			serão atrasados utilizando NOP.
 */
void delay_cycles(uint16_t cycles)
{
  /*lint -save -e522 function lacks side effect. */
  while(cycles > 100)
  {
    delay_100cycles();
    cycles -= 100;
  }
  while(cycles > 10)
  {
    delay_10cycles();
    cycles -= 10;
  }
  /*lint -restore */
}

/**
 * @brief   Efetua uma espera de N ciclos do CORE CLOCK, usando NOP.
 *          é um número inteiro de 32 bits, maior ou igual a 10
 *
 * @param   Cycles é valor correspondente à quantidade de ciclos que
 * 			serão atrasados utilizando NOP.
 */
void delay_cycles32(uint32_t cycles)
{
  /*lint -save -e522 function lacks side effect. */
  while(cycles>60000)
  {
    delay_cycles(60000);
    cycles -= 60000;
  }
  delay_cycles((uint16_t)cycles);
  /*lint -restore */
}


// 12/04/2019: Esta parte já estava na lib que o Wanger me enviou, não tirei.

// //
// // Efetua uma espera de N milissegundos, usando o SysTick.
// //
// void delay_ms(uint32_t ms)
// {
// 	s_milliseconds_left = ms;
//     while(s_milliseconds_left) {}
// }

/**
 * @brief   Função que retorna o valor da contagem da variável da lib de delay g_current_ms_count.
 *
 * @return   Retorna o valor atual de contagem de tempo, em milisegundos, usando o Systick.
 */
uint32_t time_now_tick()
{
	return g_current_ms_count;
}

/**
 * @brief   Calcula a diferente entre entre dois pontos de contagem.
 *
 * @param   start_count		Valor da contagem no inicio do intervalo de interesse.
 * @param   end_count		Valor da contagem no final do intervalo de interesse..
 *
 * @return   Retorna o intervalo de tempo entre duas contagens de tempo, em numero de ticks
 */
uint32_t time_diff_tick(uint32_t start_count, uint32_t end_count)
{
	if(end_count >= start_count)
		return (end_count - start_count);
	else
		return (0xFFFFFFFFu - start_count) + end_count;
}

/**
 * @brief   Calcula o valor em millisegundos equivalente a uma dada contagem.
 *
 * @param   tick_count	Contagem a ser convertida para millisegundos.
 *
 * @return   Retorna o valor da contagem passada como argumento em millisegundos.
 */
uint32_t ticks_to_ms(uint32_t tick_count)
{

	// Para evitar overflow tick_count*multiplicador < 2^32
	// Uma vez que o valor é um unsigned int de 32 bits, o valor estará dentro do seguinte range:
	//		0 ~ 4294967295 ou 2^32-1.
	// 		logo tick_count = (2^32)/multiplicador
	// Multiplicador será no mínimo 1000. Será maior que isso quando a freq está abaixo de 1KHz, pois:
	//if(freq >= 1000)
	//{
	//	multiplicador = freq;
	//}
	//else
	//{
	//	multiplicador = 1000;
	//}
	//	Analisando alguns casos:
	//	multiplicador = 1000				->			tick_count = (2^32)/1000
	//  multiplicador = 10000		       	->     	 	tick_count = (2^32)/10000
	//  multiplicador = 100000              ->      	tick_count = (2^32)/100000
	//
	//	O limiar depende da frequência que pode ter qualquer valor até aproximadamente 1200000 (quando a cpu para de responder com 12MHz):
	//

	// é necessario verificar o a relação entre divisor e multiplicador para que não zere!
	if(divisor>multiplicador)
	{
		// Como divisor é maior que multiplicador, é nitido que o tick_count sera
		// dividido por algum valor. Entretanto, se o tick_ count for dividido
		// antes de ser multiplicado, uma vez que o divisor é grande, muitos
		// digitos serão perdidos. Desta forma, para que apenas os digitos
		// certo sejam perdidos, primeiro faz-se a divisão do divisor pelo multiplicador
		// de tal forma a determinar o valor que deve realmente dividir o tick_count
		// e assim garante-se a maior precisão possível quando utilizando inteiros.
		if(tick_count<divisor/multiplicador)
		{
			return (tick_count*multiplicador)/divisor;
		}
		else
		{
			return tick_count/(divisor/multiplicador);
		}
	}
	else
	{
		// Como multiplicador é maior que divisor,
		// é nitido que o tick_count será multiplicado
		// Entretanto, se a divisão for feita no tick_count primeiro
		// uma vez que ele os dois são inteiros, será perdido alguns digitos
		// para isso, faz-se primeiro a divisão multiplicador/divisor, e
		// determina-se o fator que deve multiplicar o tick_count, sem
		// perder digitos.
		return tick_count*(multiplicador/divisor);
	}


}


//------------------------------------------------------------------------------
//
// Depuração
//
//------------------------------------------------------------------------------

#if defined(LOGICALIS_DEBUG_DELAY) && (LOGICALIS_DEBUG_DELAY)


#include <stdio.h>

/**
 * @brief Teste das rotinas de delay que utilizam NOP.
 *
 * @details Valida as contagens que as rotinas de delay fazem.
 */
void debug_delay(){

	log_init();
	uint32_t timenow=0;
   	uint32_t timethen=0;
   	uint32_t timePassed=0;

   	log_writeString("\n\rEste teste utiliza as rotinas de delay e testa a consistência das contagens\n\r");
   	log_writeString("Iniciando systick 150KHz\n\r");
   	delay_Init(150000);

   	// Initiating log_uart
    timenow=time_now_tick();

    log_writeString("Testando delay_10cycles\n\r");
    // Testes
    timenow=time_now_tick();
    delay_10cycles();
    timethen=time_now_tick();
    timePassed=time_diff_tick(timenow,timethen);
    log_writeString("Contagem do systick: ");
    log_writeU32(timePassed);
	log_write(13); //return
	log_write(10); //new line

	log_writeString("Testando delay_cycles(10000)\n\r");
	// Restarting timePassed
	timePassed=0;

    timenow=time_now_tick();
    delay_cycles(10000);
    timethen=time_now_tick();
    timePassed=time_diff_tick(timenow,timethen);
    log_writeString("Contagem do systick: ");
    log_writeU32(timePassed);
	log_write(13); //return
	log_write(10); //new line

	log_writeString("Testando delay_100cycles()\n\r");
	// Restarting timePassed
	timePassed=0;

    timenow=time_now_tick();
    delay_100cycles();
    timethen=time_now_tick();
    timePassed=time_diff_tick(timenow,timethen);
    log_writeString("Contagem do systick: ");
    log_writeU32(timePassed);
	log_write(13); //return
	log_write(10); //new line

	log_writeString("Testando delay_cycles32(100000)\n\r");
	// Restarting timePassed
	timePassed=0;

    timenow=time_now_tick();
    delay_cycles32(100000);
    timethen=time_now_tick();
    timePassed=time_diff_tick(timenow,timethen);
    log_writeString("Contagem do systick: ");
    log_writeU32(timePassed);
	log_write(13); //return
	log_write(10); //new line

    // Desativando o systick - caso contrário systick não para após o teste finalizar.
    SysTick->CTRL  = (~SysTick_CTRL_TICKINT_Msk)   &&
                     (~SysTick_CTRL_ENABLE_Msk);                         /* Enable SysTick IRQ and SysTick Timer */

    // Zerando a contagem da biblioteca
    g_current_ms_count=0;

}

/**
 * @brief Teste a conversão de alguns valores de contagem para millisegundos em
 * 			diferentes frequencias
 */
void tick_to_ms_debug()
{
	// initiate variables
	// Recebe a contagem
	uint32_t contagem=0;
	// Recebe o valor da contagem em ms
	uint32_t contagem_ms=0;

	// initiate delay
	delay_Init(100000);


	// send start message
	log_writeString("\n\rfrequência de systick: 100000");

	// teste 1
	// Pega uma referencia de contagem
	contagem = time_now_tick();
	// Converte a referência para milisegundos
	contagem_ms = ticks_to_ms(contagem);

	// Envia o valor da contagem obtida
    log_writeString("\n\rContagem : ");
    log_writeU32(contagem);
    // Envia o valor da contagem convertida para millisegundos
    log_writeString("\n\rContagem em ms : ");
    log_writeU32(contagem_ms);


	// send start message
	log_writeString("\n\rfrequência de systick: 10000");

	// initiate delay
	delay_Init(10000);

	// teste 1
	// Pega uma referencia de contagem
	contagem = time_now_tick();
	// Converte a referência para milisegundos
	contagem_ms = ticks_to_ms(contagem);

	// Envia o valor da contagem obtida
    log_writeString("\n\rContagem : ");
    log_writeU32(contagem);
    // Envia o valor da contagem convertida para millisegundos
    log_writeString("\n\rContagem em ms : ");
    log_writeU32(contagem_ms);


	// send start message
	log_writeString("\n\rfrequência de systick: 1000");

	// initiate delay
	delay_Init(1000);

	// teste 1
	// Pega uma referencia de contagem
	contagem = time_now_tick();
	// Converte a referência para milisegundos
	contagem_ms = ticks_to_ms(contagem);

	// Envia o valor da contagem obtida
    log_writeString("\n\rContagem : ");
    log_writeU32(contagem);
    // Envia o valor da contagem convertida para millisegundos
    log_writeString("\n\rContagem em ms : ");
    log_writeU32(contagem_ms);




	// send start message
	log_writeString("\n\rfrequência de systick: 100");

	// initiate delay
	delay_Init(100);

	// teste 1
	// Pega uma referencia de contagem
	contagem = time_now_tick();
	// Converte a referência para milisegundos
	contagem_ms = ticks_to_ms(contagem);

	// Envia o valor da contagem obtida
    log_writeString("\n\rContagem : ");
    log_writeU32(contagem);
    // Envia o valor da contagem convertida para millisegundos
    log_writeString("\n\rContagem em ms : ");
    log_writeU32(contagem_ms);




	// Send start message
	log_writeString("\n\rfrequência de systick: 10");

	// Initiate delay
	delay_Init(10);

	// Pega uma referencia de contagem
	contagem = time_now_tick();

	// Converte a referência para milisegundos
	contagem_ms = ticks_to_ms(contagem);

	// Envia o valor da contagem obtida
    log_writeString("\n\rContagem : ");
    log_writeU32(contagem);

    // Envia o valor da contagem convertida para millisegundos
    log_writeString("\n\rContagem em ms : ");
    log_writeU32(contagem_ms);


    // Desativando o systick - caso contrário systick não para após o teste finalizar.
    SysTick->CTRL  = (~SysTick_CTRL_TICKINT_Msk)   &&
                     (~SysTick_CTRL_ENABLE_Msk);                         /* Enable SysTick IRQ and SysTick Timer */

    // Reinicia a contagem - apesar que já é feito isso no delay_init.
    SysTick->VAL   = 0UL;

    // Zerando a contagem da biblioteca
    g_current_ms_count=0;

}

/**
 * @brief  Teste a conversão de contagens para millisegundos sem gerar erros em pontos
 * 			onde o overflow do valor de 32Kbits pode ocorrer.
 *
 * @param delay_freq consiste na frequencia que será utilizada nas interrupções do systick
 *
 * @details Esta lib valida a contagem sem erros em ponto de interesse onde ocorre overflow
 * 			do valor de contagem quando converte-se de ticks para ms.
 */
static void delay_count_overflow_debug(uint32_t delay_freq)
{
	// Demora muito tempo para chegar nos valores que causam overflow.
	// Por isso a contagem do systick será alterada para um valor alto, tal que

	// Inicia as variáveis
	// Valor que irá receber a contagem atual
	uint32_t contagem = 0;
	// Valor da contagem em millisegundos
	uint32_t contagem_millisegundos = 0;
	// Valor limiar inferior que é a fronteira para mudar a condição dentro to ticks_to_ms
	uint32_t contagem_intermediaria = 0;
	// Valor superior para testar o overflow
	uint32_t contagem_superior = 0;
	// Frequência de teste
	uint32_t frequencia_teste = 0;


	// Iniciar a frequencia
	frequencia_teste=delay_freq;


	// Criar um intervalo de 5 segundos antes de chegar no valor de interesse
	contagem_intermediaria=((4294967295U)/multiplicador)-5*frequencia_teste;
	g_current_ms_count=contagem_intermediaria;
	delay_Init(frequencia_teste);
	while(1)
	{
		contagem=time_now_tick();
		contagem_millisegundos = ticks_to_ms(contagem);
	    log_writeString("\n\rContagem: ");
	    log_writeU32(contagem);
	    log_writeString("\tContagem em ms: ");
	    log_writeU32(contagem_millisegundos);

	    if(g_current_ms_count>((4294967295U)/multiplicador)+5*frequencia_teste)
	    {
	    	// Desativando o systick - caso contrário systick não para após o teste finalizar.
	        SysTick->CTRL  = (~SysTick_CTRL_TICKINT_Msk)   &&
	                         (~SysTick_CTRL_ENABLE_Msk);                         /* Enable SysTick IRQ and SysTick Timer */

	        // Zerando a contagem da biblioteca
	        g_current_ms_count=0;
	    	break;
	    }

	}

	// Nesta parte, diferente da de cime, não é possível fazer parar a contagem apenas usando a contagem,
	// Desta forma usaremos um contato do loop de while
	uint32_t contador_loop = 0;
	// Pegar um contagem superior que esteja aproximadamente 5 segundos antes de acontecer o overflow
	contagem_superior=(4294967295U)-5*frequencia_teste;
	g_current_ms_count=contagem_superior;
	delay_Init(frequencia_teste);
	while(1)
	{
		contador_loop++;
		contagem=time_now_tick();
		contagem_millisegundos = ticks_to_ms(contagem);
	    log_writeString("\n\rContagem: ");
	    log_writeU32(contagem);
	    log_writeString("\tContagem em ms: ");
	    log_writeU32(contagem_millisegundos);
	    log_writeString("\tcontador_loop: ");
	    log_writeU32(contador_loop);
	    if(contador_loop>120)
	    {
	    	// Desativando o systick - caso contrário systick não para após o teste finalizar.
			SysTick->CTRL  = (~SysTick_CTRL_TICKINT_Msk)   &&
							 (~SysTick_CTRL_ENABLE_Msk);                         /* Enable SysTick IRQ and SysTick Timer */

			// Zerando a contagem da biblioteca
			g_current_ms_count=0;
			break;
	    }

	}

	// Quando a frequencia do delay está abaixo de 1KHz, e o valor da contagem se aproxima do valor de limiar
	// haverá um erro na conversão de ticks para millisegundos, pois para esta frequência exige que a contagem
	// seja multiplicado por um fator que é igual a 1000/frequenciaSystick. Quando o valor está próximo do overflow
	// esta multiplicação irá gerar um valor acima de 2^32-1, retornando um valor errado. Mas esta situação pode não
	// acontecer nunca na prática, pois se considerarmos uma frequencia do delay de 100Hz, o fator de correção é 10
	// (é necessário multiplicar a contagem por 10 para se obter o valor em milisegundos). Logo o erro começa a ocorrer
	// em (2^32-1)/10, em tempo real isto seria igual a (1/100)*((2^32-1)/10)/8640 = 49 dias de execução (este valor é
	// independente do valor da frequencia de delay. Este erro irá acontecer
}

/**
 * @brief  Faz testes de overflow da lib de delay passando por pontos
 *			críticos onde o overflow ocorre.
 */
void delay_overflow_debug()
{
	// Inicia a interface de log para enviar mensagens de debug via USART.
	log_init();

	// Inicia o teste com o delay configurado a 100KHz.
	log_writeString("\n\r\n\rTESTE COM DELAY DE 100KHZ\n\r");
	delay_count_overflow_debug(100000);

	// Inicia o teste com delay configurado a 10KHz.
	log_writeString("\n\r\n\rTESTE COM DELAY DE 10KHZ\n\r");
	delay_count_overflow_debug(10000);

	// Inicia o teste com delay configurado a 1Khz.
	log_writeString("\n\r\n\rTESTE COM DELAY DE 1KHZ\n\r");
	delay_count_overflow_debug(1000);

	// Inica o teste com delay configurado a 100 Hz.
	log_writeString("\n\r\n\rTESTE COM DELAY DE 100HZ\n\r");
	delay_count_overflow_debug(100);

	// Inica o teste com delay configurado a 10 Hz.
	log_writeString("\n\r\n\rTESTE COM DELAY DE 10HZ\n\r");
	delay_count_overflow_debug(10);
}


#endif // LOGICALIS_DEBUG_DELAY
