/**
\file    rtc_lib.c
\brief   Responsável por iniciar o RTC, fazer as configurações necessárias.

\details 


\b@{Histórico de Alterações:@}
    - 2019.05.16 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/
//==============================================================================
//
// Includes
//
//==============================================================================
#include "rtc_lib.h"
#include "fsl_rtc.h"

//==============================================================================
//
// Variáveis globais
//
//==============================================================================
volatile uint8_t g_RtcSecondFlag;
//==============================================================================
//
// Variáveis
//
//==============================================================================

//==============================================================================
//
// API
//
//==============================================================================



/**
 * @brief
 *
 * @param
 *
 * @return
 */
void start_RTC(void)
{
	/* Inicia estrutura para configurar rtc */
	rtc_datetime_t g_RtcTime;

	RTC_Init(RTC);

    /* Set a start date time and start RT */
    g_RtcTime.year = 2018U;
    g_RtcTime.month = 1U;
    g_RtcTime.day = 1U;
    g_RtcTime.hour = 0U;
    g_RtcTime.minute = 0;
    g_RtcTime.second = 0;

    /* Set RTC time to default */
    RTC_SetDatetime(RTC, &g_RtcTime);

    /* Enable RTC second interrupt */
    RTC_EnableInterrupts(RTC, kRTC_SecondInterruptEnable);

    /* Enable at the NVIC */
    NVIC_EnableIRQ(RTC_SEC_IRQn);


}



/**
 * @brief
 *
 * @param
 *
 * @return
 */
void set_RTC(RTC_Type *base, const rtc_datetime_t *datetime)
{
    /* Set RTC time to default */
    RTC_SetDatetime(base, datetime);
}



/**
 * @brief
 *
 * @param
 *
 * @return
 */
void get_RTC(RTC_Type *base, rtc_datetime_t *datetime)
{
	/* Get data */
	RTC_GetDatetime(base, datetime);
}



/**
 * @brief
 *
 * @param
 *
 * @return
 */
void RTC_Interrupt_Handler(void)
{
    if (RTC_GetStatusFlags(RTC) & kRTC_SecondInterruptFlag)
    {
        g_RtcSecondFlag = 1;
        /* Clear second interrupt flag */
        RTC_ClearStatusFlags(RTC, kRTC_SecondInterruptFlag);
    }
}


/**
 * @brief Retorna o timestamp em 5Bytes
 *
 * @param
 *
 * @return
 */
uint32_t get_timestamp(RTC_Type *base)
{
	rtc_datetime_t g_RtcTime;

	/* dados contidos no rtc_datetime_t */

    // year;  /*!< Range from 1970 to 2099.*/
    				/*
    				 * 1970 - ‭011110110010‬
    				 * 2099 - ‭100000110011 - 12 bits - reduzido para 6 bits‬
    				 */

    // month;  /*!< Range from 1 to 12.*/
    				/*
    				 * 1  - 1
    				 * 2  - 10
    				 * 12 - 1100 - 4 bits
    				 */

    // day;    /*!< Range from 1 to 31 (depending on month).*/
    				/*
    				 * 1  - 1
    				 * 2  - 10
    				 * 31 - ‭00011111 - 5 bits‬
    				 */

    // hour;   /*!< Range from 0 to 23.*/
    				/*
    				 * 0  - 0
    				 * 1  - 1
    				 * 2  - 10
    				 * 23 - ‭00010111‬ - 5 bits
    				 */

    // minute; /*!< Range from 0 to 59.*/
					/*
					 * 0   - 0
					 * 1   - 1
					 * 2   - 10
					 * 59  - ‭00111011 - 6 bits‬
					 */

    // second; /*!< Range from 0 to 59.*/
					/*
					 * 0   - 0
					 * 1   - 1
					 * 2   - 10
					 * 59  - ‭00111011 - 6 bits‬
					 */



    /* Juntando todos os valores:
     *
     * valor1 = (8 bits = [year month(b3b2)])
     * valor2 = (8 bits = [month(b1b0) day hour(b4)])
     * valor3 = (8 bits = [hour(b3b2b1b0) minute(b5b4b3b2)])
     * valor4 = (8 bits = [minute(b1b0) second])
     *
     * timestamp = [valor 1 valor 2 valor 3 valor 4]
     *
     */

	/* Get data */
	RTC_GetDatetime(base, &g_RtcTime);

	/* Pega as valores de data e horário */
	uint16_t ano_16bits = g_RtcTime.year;
	uint8_t ano_8bits;

	/* Para reduzir a quantidade de bits do ano, pega-se a diferença do ano atual com relaçao 1 2018 e codigo em 8bits */
	// Esta lib espera que o ano seja maior que 2018 e que nao passe de 2081
	if (ano_16bits<2018)
	{
		ano_8bits = (uint8_t)(2018 - ano_16bits); // Esta condição foi adicionada para gerar erro, mas esta situação não irá acontecer
	}
	else
	{
		ano_8bits = (uint8_t)(ano_16bits - 2018);
	}


	uint8_t mes = g_RtcTime.month;
	uint8_t dia = g_RtcTime.day;
	uint8_t hora = g_RtcTime.hour;
	uint8_t minuto = g_RtcTime.minute;
	uint8_t segundo = g_RtcTime.second;

	/* Forma o valor 1 */
	uint8_t valor_1 = 0x0;
	valor_1 = 0x0 | ((ano_8bits & 0x3F) << 2) | ((mes >> 2) & 0x3);//valor1 = (8 bits = [year month(b3b2)])

	/* Forma o valor 2 */
	uint8_t valor_2 = 0x0;
	valor_2 = 0x0 | ((mes & 0x3) << 6 ) | (dia << 1) | ((hora >> 4) & 0x1); // valor2 = (8 bits = [month(b1b0) day hour(b4)])

	/* Forma o valor 3 */
	uint8_t valor_3 = 0x0;
	valor_3 = 0x0 | ((hora & 0xF) << 4) | ((minuto >> 2) & 0xF); // valor3 = (8 bits = [hour(b3b2b1b0) minute(b5b4b3b2)])

	/* Forma o valor 4 */
	uint8_t valor_4 = 0x0;
	valor_4 = 0x0 | ((minuto & 0x3) << 6) | (segundo & 0x3F); // valor4 = (8 bits = [minute(b1b0) second])

	/* Transform timestamp into 4bytes */
	uint32_t timestamp = 0x0 | (valor_1 << 24 | valor_2 << 16 | valor_3 << 8 | valor_4);


	return timestamp;
}

void get_data_from_timestamp(rtc_datetime_t *datetime_from_timestamp, uint32_t timestamp_r)
{


	datetime_from_timestamp->year  = (uint16_t)(2018U + (timestamp_r >> 26));
	datetime_from_timestamp->month = (uint8_t)((timestamp_r >> 22) & 0xF); //4
	datetime_from_timestamp->day = (uint8_t)((timestamp_r >> 17) & 0x1F); //5
	datetime_from_timestamp->hour = (uint8_t)((timestamp_r >> 12) & 0x1F); //5
	datetime_from_timestamp->minute = (uint8_t)((timestamp_r >> 6) & 0x3F); //6
	datetime_from_timestamp->second = (uint8_t)((timestamp_r) & 0x3F);	//6


}

#if defined(LOGICALIS_DEBUG_RTC_LIB) && (LOGICALIS_DEBUG_RTC_LIB)


	rtc_datetime_t g_RtcTime;

	/**
	 * @brief
	 *
	 * @param
	 *
	 * @return
	 */
	void rtc_lib_debug(void)
	{
		g_RtcSecondFlag = 0;
		while (1)
		{
			if (g_RtcSecondFlag)
			{
				g_RtcSecondFlag = 0;
				/* Get data */
				RTC_GetDatetime(RTC, &g_RtcTime);
				/* Show data */
				printf("Current datetime: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n", g_RtcTime.year, g_RtcTime.month,
					   g_RtcTime.day, g_RtcTime.hour, g_RtcTime.minute, g_RtcTime.second);
			}
		}
	}



	void rtc_timestamp_debug(void)
	{
		g_RtcSecondFlag = 0;
		while (1)
		{
			if (g_RtcSecondFlag)
			{
				g_RtcSecondFlag = 0;
				/* Get data */
				/* Show data */
				RTC_GetDatetime(RTC, &g_RtcTime);
				uint32_t timestamp_reduced = get_timestamp(RTC);
				rtc_datetime_t datetime_from_timestamp;

				get_data_from_timestamp(&datetime_from_timestamp, timestamp_reduced);
				printf("Current datetime: %04hd-%02hd-%02hd %02hd:%02hd:%02hd \t Time Stamp: %x \t Datetime from timestamp: %04hd-%02hd-%02hd %02hd:%02hd:%02hd \r\n",
					   g_RtcTime.year, g_RtcTime.month,
					   g_RtcTime.day, g_RtcTime.hour,
					   g_RtcTime.minute, g_RtcTime.second,
					   timestamp_reduced, datetime_from_timestamp.year, datetime_from_timestamp.month,
					   datetime_from_timestamp.day, datetime_from_timestamp.hour,
					   datetime_from_timestamp.minute, datetime_from_timestamp.second);

			}
		}
	}




#endif // LOGICALIS_DEBUG_RTC_LIB
