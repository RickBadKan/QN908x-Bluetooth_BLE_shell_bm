/**
\file    rtc_lib.h
\brief   Responsável por iniciar o RTC, fazer as configurações necessárias.

\details 


\b@{Histórico de Alterações:@}
    - 2019.05.16 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/

#ifndef RTC_LIB_H_
#define RTC_LIB_H_


#include "fsl_rtc.h"


#define LOGICALIS_DEBUG_RTC_LIB 1





//==============================================================================
//
// Variáveis globais
//
//==============================================================================

//==============================================================================
//
// Constantes e estruturas de dados
//
//==============================================================================

//
// Estrutura de configuração 
//


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
void start_RTC(void);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void set_RTC(RTC_Type *base, const rtc_datetime_t *datetime);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void get_RTC(RTC_Type *base, rtc_datetime_t *datetime);
uint32_t get_timestamp(RTC_Type *base);
void get_data_from_timestamp(rtc_datetime_t *datetime_from_timestamp, uint32_t timestamp);
/**
 * @brief
 *
 * @param
 *
 * @return
 */
void RTC_Interrupt_Handler(void);

#if defined(LOGICALIS_DEBUG_RTC_LIB) && (LOGICALIS_DEBUG_RTC_LIB)

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void rtc_lib_debug(void);

void rtc_timestamp_debug(void);

#endif // LOGICALIS_DEBUG_RTC_LIB


#endif /* RTC_LIB_H_ */


