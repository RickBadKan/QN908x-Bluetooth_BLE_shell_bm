/*******************************************************************************
delay.h

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
#ifndef H_DELAY
#define H_DELAY

#include <stdint.h>
#include "fsl_clock.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus



//==============================================================================
//
// Variáveis globais
//
//==============================================================================

//
// Contador de milissegundos, incrementado pelo SysTick
//
extern volatile uint32_t g_current_ms_count;

/*! Esta parte do codigo já estava no versão que o Wagner enviou.
 * Contador de milissegundos a esperar, decrementado em delay_ms()
 */
//extern volatile uint32_t s_milliseconds_left;



//==============================================================================
//
// Macros e constantes de parametrização e apoio geral
//
//==============================================================================

//
// Frequência do CORE CLOCK, em Hz
//
#define WAIT_CORE_CLOCK_HZ               SystemCoreClock // CLOCK_GetCoreSysClkFreq()

/**
 * @brief   Cálculo da quantidade de ciclos por unidade de tempo, com o clock informado.
 *
 * @param 	ms o tempo a ser esperado em millisegundos.
 *			us o tempo a ser esperado em microsegundos
 *			ns o tempo a ser esperado em nanosegundos
 *			hz a frequência do clock do sistema.
 * @detail  Usado em delay_us(), delay_ns() e delay_ms():
 */
#define WAIT_NUMOFCYCLES_PER_MS(ms, hz)  ((ms)*((hz)/1000))
#define WAIT_NUMOFCYCLES_PER_US(us, hz)  ((us)*(((hz)/1000)/1000))
#define WAIT_NUMOFCYCLES_PER_NS(ns, hz)  (((ns)*(((hz)/1000)/1000))/1000)

/**
 * @brief   Efetua uma espera de N ciclos do CORE CLOCK, onde N precisa ser >= 10.
 *
 * @param 	cycles consiste na quantidade de ciclos que são aguardados.
 *
 * @detail  Usado em delay_us(), delay_ns() e delay_ms():
 */
#define WAIT_C(cycles)   ( \
                           (cycles <= 10) \
                             ? delay_10cycles() \
                             :( \
                                (cycles <= 0xFFFF) \
                                ? delay_cycles((uint16_t)cycles) \
                                : delay_cycles32((uint32_t)cycles) \
                              ) \
                         )


//==============================================================================
//
// API
//
//==============================================================================

/**
 * @brief   Inicia as configurações para incia as interrupções do systick.
 *
 * @param 	freq consiste na frequência em que o systick irá interrupções.
 *
 * @detail o Systick é utilizado nas funções:
 *	 - delay_ms()
 *	 - time_now_ms()
 */
void delay_Init(uint32_t freq);

/**
 * @brief   Efetua uma espera de 10 ciclos do CORE CLOCK, usando NOP.
 *
 */
void delay_10cycles();

/**
 * @brief   Efetua uma espera de 100 ciclos do CORE CLOCK, usando NOP.
 *
 */
void delay_100cycles();

/**
 * @brief   Efetua uma espera de N ciclos do CORE CLOCK, usando NOP.
 *          N é um número inteiro de 16 bits, maior ou igual a 10.
 *
 * @param   Cycles é valor correspondente à quantidade de ciclos que
 * 			serão atrasados utilizando NOP.
 */
void delay_cycles(uint16_t cycles);

/**
 * @brief   Efetua uma espera de N ciclos do CORE CLOCK, usando NOP.
 *          é um número inteiro de 32 bits, maior ou igual a 10
 *
 * @param   Cycles é valor correspondente à quantidade de ciclos que
 * 			serão atrasados utilizando NOP.
 */
void delay_cycles32(uint32_t cycles);

// Essa parte do código já tinha no código que o Wagner enviou.
// Efetua uma espera de N milissegundos, usando o SysTick.
//
//void delay_ms(uint32_t ms);

/**
 * @brief   Efetua uma espera de N milissegundos, usando o SysTick.
 *
 * @param   ms	Valor do atraso desejado em millisegundos.
 *
 * @details Delay máximo = 2^32 / (Clock / 1.000) ms
 *
 * 			Requer que a configuração do clock seja estática (não mude
 * 			o clock ao longo da execução).
 *
 * 			Lógica:
 * 				O tempo é igual a 0 ?
 * 					Sim: então não faz nada e encerra
 * 					Não: então faz delay e encerra
 */
#define delay_ms(ms)  \
        /*lint -save -e(505,506,522) Ver comentário no topo deste arquivo*/ \
        ( \
          ((WAIT_NUMOFCYCLES_PER_MS((ms), WAIT_CORE_CLOCK_HZ)==0) || ((ms) == 0)) \
          ?  (void)0 \
          :  WAIT_C(WAIT_NUMOFCYCLES_PER_MS((ms), WAIT_CORE_CLOCK_HZ)) \
        /*lint -restore */ \
        )

/**
 * @brief   Efetua uma espera de N microssegundos, usando NOP.
 *
 * @param   us	Valor do atraso desejado em microsegundos.
 *
 * @details Delay máximo = 2^32 / (Clock / 1.000.000) us
 *
 * 			Requer que a configuração do clock seja estática (não mude
 * 			o clock ao longo da execução).
 *
 * 			Lógica:
 * 				O tempo é igual a 0 ?
 * 					Sim: então não faz nada e encerra
 * 					Não: então faz delay e encerra
 */
#define delay_us(us)  \
        /*lint -save -e(505,506,522) Ver comentário no topo deste arquivo*/ \
        ( \
          ((WAIT_NUMOFCYCLES_PER_US((us), WAIT_CORE_CLOCK_HZ)==0) || ((us) == 0)) \
          ?  (void)0 \
          :  WAIT_C(WAIT_NUMOFCYCLES_PER_US((us), WAIT_CORE_CLOCK_HZ)) \
        /*lint -restore */ \
        )

/**
 * @brief   Efetua uma espera de N nanosegundos, usando NOP.
 *
 * @param   ns	Valor do atraso desejado em nanosegundos.
 *
 * @details Delay máximo = 2^32 / (Clock / 1.000.000.000) ns
 *
 * 			Requer que a configuração do clock seja estática (não mude
 * 			o clock ao longo da execução).
 *
 * 			Lógica:
 * 				O tempo é igual a 0 ?
 * 					Sim: então não faz nada e encerra
 * 					Não: então faz delay e encerra
 */

#define delay_ns(ns)  \
        /*lint -save -e(505,506,522) Ver comentário no topo deste arquivo*/ \
        ( \
          ((WAIT_NUMOFCYCLES_PER_NS((ns), WAIT_CORE_CLOCK_HZ)==0)||(ns)==0) \
          ? (void)0 \
          : WAIT_C(WAIT_NUMOFCYCLES_PER_NS((ns), WAIT_CORE_CLOCK_HZ)) \
        /*lint -restore */ \
        )

/**
 * @brief   Calcula a diferente entre entre dois pontos de contagem.
 *
 * @param   start_count		Valor da contagem no inicio do intervalo de interesse.
 * @param   end_count		Valor da contagem no final do intervalo de interesse..
 *
 * @return   Retorna o intervalo de tempo entre duas contagens de tempo, em numero de ticks
 */
uint32_t time_now_tick();

//
// Retorna o intervalo de tempo entre duas contagens de tempo, em milisegundos.
//
uint32_t time_diff_tick(uint32_t start_count, uint32_t end_count);


/**
 * @brief   Calcula o valor em millisegundos equivalente a uma dada contagem.
 *
 * @param   tick_count	Contagem a ser convertida para millisegundos.
 *
 * @return   Retorna o valor da contagem passada como argumento em millisegundos.
 */
uint32_t ticks_to_ms(uint32_t tick_count);

//==============================================================================
//
// ISR's
//
//==============================================================================

/**
 * @brief   ISR do SysTick.
 *
 * @details Incrementa o contador geral de tempo e decrementa o contador de
 * 			tempo restante. Usada por time_now_ms()
 *
 */
static inline void delay_ISR_SysTick()
{
	// Incrementa o contador de milissegundos
	g_current_ms_count++;
}

//
// Depuração
//
#if defined(LOGICALIS_DEBUG_DELAY) && (LOGICALIS_DEBUG_DELAY)

/**
 * @brief Teste das rotinas de delay que utilizam NOP.
 *
 * @details Valida as contagens que as rotinas de delay fazem.
 */
void debug_delay(void);

/**
 * @brief Teste a conversão de alguns valores de contagem para millisegundos em
 * 			diferentes frequencias
 */
void tick_to_ms_debug(void);

/**
 * @brief  Faz testes de overflow da lib de delay passando por pontos
 *			críticos onde o overflow ocorre.
 */
void delay_overflow_debug(void);

#endif // LOGICALIS_DEBUG_DELAY

#if defined(__cplusplus)
}  // extern "C"
#endif // __cplusplus

#endif // H_DELAY
