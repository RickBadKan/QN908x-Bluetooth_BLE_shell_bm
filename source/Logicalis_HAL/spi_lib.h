/**
\file    spi_lib.c
\brief   Responsável por iniciar o SPI, fazer as configurações necessárias, enviar e receber.

\details 


\b@{Histórico de Alterações:@}
    - 2019.05.29 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/

#ifndef SPI_LIB_H_
#define SPI_LIB_H_

#include "fsl_spi.h"
#include "clock_config.h"
#include "delay.h"

#define LOGICALIS_DEBUG_SPI_LIB 1

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

//==============================================================================
//
// Estrutura de configuração 
//
//==============================================================================


/**
 * SPI configuration struct.
 */
typedef struct spi_configuration
{
	spi_master_config_t 	master_config;      /*!< Master Config */
	clock_name_t 			spiClock;      		/*!< SPI Clock */
	SPI_Type				*spi_type;			/*!< SPI Type (SPI0, SPI1, etc...) */
} spi_configuration_t;

//==============================================================================
//
// API
//
//==============================================================================


/**
 * @brief Inicializa o SPI master ( Não configura pinos )
 *
 * @param
 * 			spi_config - estrutura de configuração do SPI master.
 *
 * @return void
 */
void spi_init(spi_configuration_t *spi_config);


/**
 * @brief Faz a transferência de dados usando a interface SPI master (requer que a interface tenha sido configurada antes)
 *
 * @param
 *			xfer 		- Estrutura de dados com os buffers de envio e recepção
 *			spi_config 	- Estrutura de configuração da interface SPI
 * @return void
 */
void spi_TransferNonBlocking(spi_transfer_t *xfer,spi_configuration_t *spi_config);


#if defined(LOGICALIS_DEBUG_SPI_LIB) && (LOGICALIS_DEBUG_SPI_LIB)


/**
 * @brief Testa a interface SPI instanciando um master em SPI0 e um slave em SPI1
 * 		  obs; Requer interligação entre pinos da placa:
 * 		  PORTA PIN2  FC2_SCL_SSEL1 	- 	PORTA PIN13 FC3_SSEL1
 *		  PORTA PIN15 FC3_SCK 			- 	PORTA PIN29 FC2_SCK
 *		  PORTA PIN16 FC3_MOSI 			- 	PORTA PIN26 FC2_SDA_MOSI
 *		  PORTA PIN17 FC3_MISO 			- 	PORTA PIN27 FC2_SCL_MISO
 *
 * @param void
 *
 * @return void
 */
void debug_spi_interface(void);

#endif // LOGICALIS_DEBUG_SPI_LIB


#endif /* SPI_LIB_H_ */


