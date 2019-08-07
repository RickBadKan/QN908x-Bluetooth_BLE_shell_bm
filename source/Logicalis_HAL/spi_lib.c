/**
\file    spi_lib.c
\brief   Responsável por iniciar o SPI como mestre, fazer as configurações necessárias, enviar e receber.

\details 
	Como utilizar:
		1) Definir os pinos do SPI e configurá-los no pin_mux antes de utilizar esta biblioteca.
		2) Instanciar uma estrutura de configuração do tipo spi_configuration_t, como exemplo abaixo:
			//
			spi_master_config_t masterConfig = {0};

			//
			SPI_MasterGetDefaultConfig(&masterConfig);

			//
			spi_configuration_t spi_debug_config =
			{
				.master_config =
				{
					.enableLoopback = masterConfig.enableLoopback,
					.enableMaster = masterConfig.enableMaster,
					.polarity = masterConfig.polarity,
					.phase = masterConfig.phase,
					.direction = masterConfig.direction,
					.baudRate_Bps = masterConfig.baudRate_Bps,
					.dataWidth = masterConfig.dataWidth,
					.sselNum = 1,
					.sselPol = (kSPI_SpolActiveAllLow | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh),
					.txWatermark = masterConfig.txWatermark,
					.rxWatermark = masterConfig.rxWatermark
				},
				.spiClock= kCLOCK_BusClk,
				.spi_type=SPI0
			};
		3) Inicializa a interface SPI através do comando:
			spi_init(&spi_debug_config);

		4) Para transferir dados via SPI, faz o seguinte procedimento:

			    spi_transfer_t xfer = {0};

    			xfer.txData = srcBuff;
    			xfer.dataSize = sizeof(srcBuff);
    			xfer.rxData = masterFromSlaveBuff;
    			xfer.configFlags |= kSPI_FrameAssert;
    			spi_TransferNonBlocking(&xfer,&spi_debug_config);

\b@{Histórico de Alterações:@}
    - 2019.05.29 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/
//==============================================================================
//
// Includes
//
//==============================================================================
#include "spi_lib.h"

#include "fsl_spi.h"
#include "delay.h"

//==============================================================================
//
// Variáveis globais
//
//==============================================================================

//==============================================================================
//
// Variáveis
//
//==============================================================================


static spi_master_handle_t masterHandle;
//==============================================================================
//
// API
//
//==============================================================================
static volatile bool masterFinished = false;


/**
 * @brief Função de callback da transferência sem block do SPI
 *
 * @param Os params são passados pela função para a qual essa callback é passada.
 *			base		-
 *			handle		-
 *			status		-
 *			userData	-
 *
 * @return void
 */
static void masterCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    masterFinished = true;
}


/**
 * @brief Inicializa o SPI master ( Não configura pinos )
 *
 * @param
 * 			spi_config - estrutura de configuração do SPI master.
 *
 * @return void
 */
void spi_init(spi_configuration_t *spi_config)
{


    /* Configuração básica
     *
     * 	 Configure RST_SW_SET and RST_SW_CLR registers to reset the flexcomm.
     * 	 Configure the required flexcomm pin functions through IOCON.
     *	 Configure the SPI for receiving and transmitting data:
     *		-> Configure CLK_EN register to enable clock for the related flexcomm.
     *		-> Select the desired flexcomm function by writing to the PSELID register of the related flexcomm.
     *		-> Enable or disable the related flexcomm interrupts in the NVIC.
     *		-> Configure the FIFOs for operation.
     *		-> Configure the flexcomm interface clock and SPI data rate.
     *		-> Set the RXIGNORE bit to only transmit data and not read the incoming data. Otherwise, the transmit halts when the FIFORD buffer is full.
     *
     */

    SPI_MasterInit(spi_config->spi_type, &spi_config->master_config,CLOCK_GetFreq(spi_config->spiClock));


    /* Create handle for SPI master */
    SPI_MasterTransferCreateHandle(spi_config->spi_type, &masterHandle, masterCallback, NULL);
}

/**
 * @brief Faz a transferência de dados usando a interface SPI master (requer que a interface tenha sido configurada antes)
 *
 * @param
 *			xfer 		- Estrutura de dados com os buffers de envio e recepção
 *			spi_config 	- Estrutura de configuração da interface SPI
 * @return void
 */
void spi_TransferNonBlocking(spi_transfer_t *xfer,spi_configuration_t *spi_config)
{
	SPI_MasterTransferNonBlocking(spi_config->spi_type, &masterHandle, xfer);
	while (masterFinished != true)
	{

	}
	masterFinished = false;
}

#if defined(LOGICALIS_DEBUG_SPI_LIB) && (LOGICALIS_DEBUG_SPI_LIB)

#include <stdio.h>
#include "spi_lib.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_SPI_MASTER_IRQ FLEXCOMM2_IRQn
#define EXAMPLE_SPI_MASTER_CLK_SRC kCLOCK_BusClk
#define EXAMPLE_SPI_MASTER_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define EXAMPLE_SPI_SLAVE SPI1
#define EXAMPLE_SPI_SLAVE_IRQ FLEXCOMM3_IRQn
#define EXAMPLE_SPI_SSEL 1

/*!
 * Master Mode:
 * Select polarities of the used and unused chip selects.
 * Used Chip Select polarity:
 * CS1 - Active LOW
 * Unused Chip Select polarities :
 * CS0 - Active LOW
 * CS2 - Active HIGH
 * CS3 - Active HIGH
 */
#define EXAMPLE_MASTER_SPI_SPOL (kSPI_SpolActiveAllLow | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh)
/*!
 * Slave Mode:
 * Select polarities of the used and unused chip selects.
 * Used Chip Select polarity:
 * CS1 - Active LOW
 * Unused Chip Select polarities :
 * CS0 - Active HIGH
 * CS2 - Active HIGH
 * CS3 - Active HIGH
 */
#define EXAMPLE_SLAVE_SPI_SPOL \
    (kSPI_SpolActiveAllLow | kSPI_Spol0ActiveHigh | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh)


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];

static uint8_t slaveToMasterBuff[BUFFER_SIZE];
static uint8_t masterFromSlaveBuff[BUFFER_SIZE];

static spi_slave_handle_t slaveHandle;
static volatile bool slaveFinished = false;



static void slaveCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData)
{
    slaveFinished = true;
}

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
void debug_spi_interface(void)
{
    uint32_t i = 0U;
    uint32_t err = 0U;

	// Inicializa uma estrutura do spi_master_config_t
	spi_master_config_t masterConfig = {0};

	// Pega a configuração padrão. Por uma questão de conveniência, a variável masterconfig é utilizada para definir as propriedades na config do SPI
	SPI_MasterGetDefaultConfig(&masterConfig);

	// Inicializa a configuração da SPI master.
	spi_configuration_t spi_debug_config =
	{
	    .master_config =
	    {
	    	.enableLoopback = masterConfig.enableLoopback,      									/*!< Enable loopback for test purpose */
	        .enableMaster = masterConfig.enableMaster,          									/*!< Enable SPI at initialization time */
	        .polarity = masterConfig.polarity,     													/*!< Clock polarity */
	        .phase = masterConfig.phase,           													/*!< Clock phase */
	        .direction = masterConfig.direction,   													/*!< MSB or LSB */
	        .baudRate_Bps = masterConfig.baudRate_Bps,          									/*!< Baud Rate for SPI in Hz */
	        .dataWidth = masterConfig.dataWidth,        											/*!< Width of the data */
	        .sselNum = 1,					                										/*!< Slave select number */
	        .sselPol = (kSPI_SpolActiveAllLow | kSPI_Spol2ActiveHigh | kSPI_Spol3ActiveHigh),  		/*!< Configure active CS polarity */
	        .txWatermark = masterConfig.txWatermark,												/*!< txFIFO watermark */
	        .rxWatermark = masterConfig.rxWatermark												/*!< rxFIFO watermark */
	    },
		.spiClock= kCLOCK_BusClk,
		.spi_type=SPI0
	};


    spi_init(&spi_debug_config);


    spi_slave_config_t slaveConfig = {0};
    spi_transfer_t xfer = {0};

    /* Init SPI slave */
    /*
     * slaveConfig.polarity = flexSPI_ClockPolarityActiveHigh;
     * slaveConfig.phase = flexSPI_ClockPhaseFirstEdge;
     * slaveConfig.direction = flexSPI_MsbFirst;
     * slaveConfig.dataWidth = flexSPI_Data8Bits;
     */
    SPI_SlaveGetDefaultConfig(&slaveConfig);
    slaveConfig.sselPol = (spi_spol_t)EXAMPLE_SLAVE_SPI_SPOL;
    SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &slaveConfig);
    SPI_SlaveTransferCreateHandle(EXAMPLE_SPI_SLAVE, &slaveHandle, slaveCallback, NULL);


    /* Set priority, slave have higher priority */
    NVIC_SetPriority(EXAMPLE_SPI_MASTER_IRQ, 1U);
    NVIC_SetPriority(EXAMPLE_SPI_SLAVE_IRQ, 0U);


    /* Init source buffer */
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        srcBuff[i] = i;
    }


    /* SPI slave transfer */
    xfer.rxData = destBuff;
    xfer.dataSize = sizeof(destBuff);
    xfer.txData = slaveToMasterBuff;
    SPI_SlaveTransferNonBlocking(EXAMPLE_SPI_SLAVE, &slaveHandle, &xfer);


    /* SPI master start transfer */
    xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = masterFromSlaveBuff;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);


    while (slaveFinished != true)
    {

    }

    for (i = 0U; i < BUFFER_SIZE; i++)
    {
    	printf("%d  ",destBuff[i]);
    }

    /* Check the data received */
    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        if (destBuff[i] != srcBuff[i])
        {
            printf("\r\nThe %d data is wrong, the data received is %d \r\n", i, destBuff[i]);
            err++;
        }
    }
    if (err == 0U)
    {
        printf("\r\nSPI transfer finished!\r\n");

    }

}

#endif // LOGICALIS_DEBUG_SPI_LIB
