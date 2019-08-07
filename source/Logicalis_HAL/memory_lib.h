/**
\file    memory_lib.h
\brief   Responsável pelas leituras e escritas em uma memória.

\details Antes de utilizar, definir:
	1. NUMBER_OF_SECTORS


\b@{Histórico de Alterações:@}
    - 2019.05.08 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/

#ifndef MEMORY_LIB_H_
#define MEMORY_LIB_H_


#include "mx25r_flash.h"

//==============================================================================
//
// Variáveis globais
//
//==============================================================================
#define LOGICALIS_DEBUG_MEMORY_LIB 1


#define SECTOR_ADDR 0
#define SECTOR_SIZE 4096
#define FLASH_SPI_SSEL 0
#define EXAMPLE_SPI_MASTER_IRQn FLEXCOMM2_IRQn
#define EXAMPLE_SPI_MASTER_CLK_SRC kCLOCK_BusClk
#define EXAMPLE_SPI_MASTER_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)


#define NUMBER_OF_SECTORS 64
#define SECTOR_SIZE 4096

//==============================================================================
//
// Constantes e estruturas de dados
//
//==============================================================================

//
// Estrutura de configuração do powerDownMode 
//
typedef struct _flash_powerdown_config
{
	uint32_t			    lowPwr; //Flag indicating flash in on lowPwr mode
	uint32_t			    tDPDD;
	uint32_t			    tCRDP;
    uint32_t			    tDP;

} flash_powerdown_config_t;

/*!
 * Select polarities of the used and unused chip selects.
 * Used Chip Select polarity:
 * CS0 - Active LOW
 * Unused Chip Select polarities :
 * CS1 - Active LOW
 * CS2 - Active HIGH
 * CS3 - Active HIGH
 */
#define EXAMPLE_SPI_SPOL (kSPI_SpolActiveAllLow | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh)
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


/**
 * @brief Callback utilizado para iniciar a memória flash
 *
 * @param
 *
 * @return
 */
int flash_transfer_cb(void *transfer_prv, uint8_t *tx_data, uint8_t *rx_data, uint32_t dataSize, bool eof);

/**
 * @brief Função que inicia
 *
 * @param masterCallback callback acionado após transferência para a memória terminar.
 *
 */
int memory_init(SPI_Type *base);

/**
 * @brief Escreve buffer no endereço de memória passado como argumento.
 *
 * @param
 *
 * @return
 */
mx25r_err_t memory_write(uint32_t address, char *buffer, char size);

/**
 * @brief Lê o endenreço de memória passado como argumento e salva no buffer.
 *
 * @param
 *
 * @return
 */
mx25r_err_t memory_read(uint32_t address, char *buffer, char size);

/**
 * @brief Apaga o setor de memória no endereço passado como argumento.
 *
 * @param
 *
 * @return
 */
int memory_erase_sector(uint32_t address);


/**
 * @brief
 *
 * @param
 *
 * @return
 */
int enterPowerDown(flash_powerdown_config_t *pwrDwnConfig);

/**
 * @brief
 *
 * @param
 *
 * @return
 */
mx25r_err_t enterStandBy(flash_powerdown_config_t *pwrDwnConfig);

#if defined(LOGICALIS_DEBUG_MEMORY_LIB) && (LOGICALIS_DEBUG_MEMORY_LIB)

void memory_lib_debug_write_read(uint32_t setor_tested);


#endif // LOGICALIS_DEBUG_MEMORY_LIB


#endif /* MEMORY_LIB_H_ */


