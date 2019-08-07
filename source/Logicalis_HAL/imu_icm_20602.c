/**
\file    imu_icm_20602.c
\brief   Responsável por iniciar o SPI, fazer as configurações necessárias, enviar e receber.

\details 


\b@{Histórico de Alterações:@}
    - 2019.06.07 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/
//==============================================================================
//
// Includes
//
//==============================================================================
#include "imu_icm_20602.h"
#include "spi_lib.h"

#include "delay.h"
//==============================================================================
//
// Variáveis globais
//
//==============================================================================
#define escrever 	0x00U
#define ler			0x80U

#define UINT8_TO_INT16(dst, src_high, src_low) \
  do { \
    dst = (src_high); \
    dst <<= 8; \
    dst |= (src_low); \
  } while (0);

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
void imu_icm_20602_init(void)
{
/*
 * 13) Levantar as características da comunicação SPI da IMU da coleira:
	Data is delivered MSB first and LSB last				-
	Data is latched on the rising edge of SPC				-
	Data should be transitioned on the falling edge of SPC	-
	The maximum frequency of SPC is 10MHz					-
	SPI read and write operations are completed in 16 or more clock cycles (two or more bytes):
		- The first byte contains the SPI Address
		- The following byte(s) contain(s) the SPI data:
			- The first bit of the first byte contains the Read/Write bit and indicates the Read (1) or Write (0) operation.
			- The following 7 bits contain the Register Address
			- In cases of multiple-byte Read/Writes, data is two or more bytes:

		- Formato:
		MSB                          LSB
		R/W  A6  A5  A4  A3  A2  A1  A0

		MSB                          LSB
		D7   D6  D5  D4  D3  D2  D1  D0
		-

13) Verificar, usando o exemplo do I2C, qual é impacto na transmissão do SPI visto que é MSB first and LSB last:
	A princípio, o formato que a IMU recebe é o formato mais lógico.

14) Verificar a configuração da IMU de acordo com o criterio: Data is latched on the rising edge of SPC, Data should be transitioned on the falling edge of SPC
	Poderia ser:
	CPOL=1 CPHA=1

	Depende do estado do clock quando em rest:
		Pag. 15 datasheet IMU, mostra o clock em 1 quando em rest.

	Logo: CPOL=1 CPHA=1
 */


	// Deve receber qual spi utilizar.
	

	// Deve receber qual GPIO é o aviso do FIFO cheio
	

	// Envia comandos para iniciliar o IMU
	

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
	        .polarity = kSPI_ClockPolarityActiveLow,     											/*!< Clock polarity */
	        .phase = kSPI_ClockPhaseSecondEdge,           											/*!< Clock phase */
	        .direction = kSPI_MsbFirst,   															/*!< MSB or LSB */
	        .baudRate_Bps = 500000,          														/*!< Baud Rate for SPI in Hz */
	        .dataWidth = masterConfig.dataWidth,        											/*!< Width of the data */
	        .sselNum = kSPI_Ssel0,					                										/*!< Slave select number */
	        .sselPol = (kSPI_SpolActiveAllLow),  													/*!< Configure active CS polarity */
	        .txWatermark = masterConfig.txWatermark,												/*!< txFIFO watermark */
	        .rxWatermark = masterConfig.rxWatermark													/*!< rxFIFO watermark */
	    },
		.spiClock= kCLOCK_BusClk,
		.spi_type=SPI1 // SPI da FLEXCOMM3
	};

	spi_init(&spi_debug_config);
	spi_transfer_t xfer = {0};

	uint8_t srcBuff[2];
	uint8_t dstBuff[2];

    /* SPI master start transfer */

	//Dados chegando
	/*
	 * Read (1) or Write (0)
	 *
	 * 	MSB                          LSB
		R/W  A6  A5  A4  A3  A2  A1  A0

		MSB                          LSB
		D7   D6  D5  D4  D3  D2  D1  D0
	 */

	// TODO: Definir a ordem dos dois bytes de comando para o ICM
	/* Para garantir que o valor do destino não seja coincidentemente igual a 0x12 no momento do reconhecimento */
	dstBuff[0] = 0xEFU;
	dstBuff[1] = 0xABU;


    // Ler who-am-i
	srcBuff[0] = ler | 0x75U;	// ::: Ler who-am-i - registrador 75
	srcBuff[1] = 0xFFU;			// QUALQUER COISA



	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(dstBuff);
    xfer.rxData = dstBuff;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    /* O valor padrão no registrador who am i do icm 20602 é 0x12*/
    if (dstBuff[1] == 0x12)
    {
    	printf("\n\r Sucesso ao comunicar ICM 20602");
    }
    else
    {
    	printf("\n\r Erro ao comunicar com ICM 20602");
    }

    xfer.txData = NULL;
    xfer.dataSize = sizeof(dstBuff);
    xfer.rxData = dstBuff;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    // General Procedure:
	//  1. reset chip
	srcBuff[0] = escrever | 0x6BU;	//‭ ::: Comando de reset - registrado 6B
	srcBuff[1] = 0x80U;			//

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL; // Para o comando de reset não precisa de buffer de recepção de dados.
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    /* Espera alguns milisegundos para dar tempo de o dispositivo finalizar o reset */
    delay_ms(1000);

    //  2. set clock for PLL for optimum performance as documented in datasheet
    // 	From DS-000176-ICM-20602-v1.0.pdf (pg 46):
    //		Auto selects the best available clock source – PLL if ready, else use the Internal oscillator
	srcBuff[0] = escrever | 0x6BU;	//‭ ::: Comando de reset
	srcBuff[1] = 0x01U;				//

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    //  3. place accelerometer and gyroscope into standby
	srcBuff[0] = escrever | 0x6CU;	//‭ ::: Registrador PWR_MGMT_2
	srcBuff[1] = 0x3FU;				// Desabilita acelerometro e giroscopio

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    //  4. disable fifo
	srcBuff[0] = escrever | 0x6AU;	//‭ ::: FIFO
	srcBuff[1] = 0x00U;				//

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    //  . disable I2C
	srcBuff[0] = escrever | 0x70U;	//‭ :::
	srcBuff[1] = 0x40U;				// Disable I2C Slave module and put the serial interface in SPI mode only.

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    //  5. configure chip

    // ACCEL_CONFIG - 1C
	// ACCEL_CONFIG2 - 1D

    //	ACCEL_CONFIG - 1C
    // 		ACCEL_FS_SEL:  ±2g (00), ±4g (01), ±8g (10), ±16g (11)
    //
	srcBuff[0] = escrever | 0x1CU;					// :::
	srcBuff[1] = 0x03U << ACCEL_FS_SEL_SHIFT;		//

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);

    //	ACCEL_CONFIG2 - 1D
    // 		Samples average:
    //				0 = Average 4 samples
    //				1 = Average 8 samples
    //				2 = Average 16 samples
    //				3 = Average 32 samples
    //
    // 		ACCEL_FCHOICE_B - (0) DLPF ativo (1) DLPF inativo
    //
	srcBuff[0] = escrever | 0x1DU;	//‭ :::
	srcBuff[1] = (0x00U << DEC2_CFG_SHIFT) | (0x00 << ACCEL_FCHOICE_B_SHIFT);// 1000 0000

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);


    //  6. enable accelerometer and gyroscope
	srcBuff[0] = escrever | 0x6CU;	//‭ ::: Registrador PWR_MGMT_2
	srcBuff[1] = 0x07U;				// Habilita acelerometro

	xfer.txData = srcBuff;
    xfer.dataSize = sizeof(srcBuff);
    xfer.rxData = NULL;
    xfer.configFlags |= kSPI_FrameAssert;
    spi_TransferNonBlocking(&xfer,&spi_debug_config);


}

void imu_icm_20602_read_loop(void)
{
	// TODO: O init deve receber a configuração, assim como o imu_icm_20602_read_loop

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
	        .polarity = kSPI_ClockPolarityActiveLow,     											/*!< Clock polarity */
	        .phase = kSPI_ClockPhaseSecondEdge,           											/*!< Clock phase */
	        .direction = kSPI_MsbFirst,   															/*!< MSB or LSB */
	        .baudRate_Bps = 500000,          														/*!< Baud Rate for SPI in Hz */
	        .dataWidth = masterConfig.dataWidth,        											/*!< Width of the data */
	        .sselNum = kSPI_Ssel0,					                										/*!< Slave select number */
	        .sselPol = (kSPI_SpolActiveAllLow),  													/*!< Configure active CS polarity */
	        .txWatermark = masterConfig.txWatermark,												/*!< txFIFO watermark */
	        .rxWatermark = masterConfig.rxWatermark													/*!< rxFIFO watermark */
	    },
		.spiClock= kCLOCK_BusClk,
		.spi_type=SPI1 // SPI da FLEXCOMM3
	};


	spi_transfer_t xfer = {0};

	uint8_t srcBuff[2];
	uint8_t dstBuff[7];

	int16_t acel_x;
	int16_t acel_y;
	int16_t acel_z;

	int16_t temperature;

	while(1)
	{

		// Definindo o vetor.
		srcBuff[0] = 0x11U;
		srcBuff[1] = 0x22U;

	    //  x
		srcBuff[0] = ler | 0x3BU;	//‭ ::: Ler ACCEL_XOUT_H
		srcBuff[1] = 0x00U;			//

		xfer.txData = srcBuff;
	    xfer.dataSize = sizeof(dstBuff);
	    xfer.rxData = dstBuff;
	    xfer.configFlags |= kSPI_FrameAssert;
	    spi_TransferNonBlocking(&xfer,&spi_debug_config);
//        printf("\n%x \t%x \t%x \t%x \t%x \t%x \t%x",dstBuff[0],dstBuff[1],dstBuff[2],dstBuff[3],dstBuff[4],dstBuff[5],dstBuff[6]);

//		srcBuff[0] = ler | 0x3CU;	//‭ ::: Ler ACCEL_XOUT_H
//		srcBuff[1] = 0x00U;			//
//
//		xfer.txData = srcBuff;
//	    xfer.dataSize = sizeof(dstBuff);
//	    xfer.rxData = dstBuff;
//	    xfer.configFlags |= kSPI_FrameAssert;
//	    spi_TransferNonBlocking(&xfer,&spi_debug_config);
//	    printf("\t%x %x ",dstBuff[2],dstBuff[3]);
//
//	    srcBuff[0] = ler | 0x3DU;	//‭ ::: Ler ACCEL_XOUT_H
//		srcBuff[1] = 0x00U;			//
//
//		xfer.txData = srcBuff;
//	    xfer.dataSize = sizeof(dstBuff);
//	    xfer.rxData = dstBuff;
//	    xfer.configFlags |= kSPI_FrameAssert;
//	    spi_TransferNonBlocking(&xfer,&spi_debug_config);
//	    printf("\t%x %x \n",dstBuff[4],dstBuff[5]);

	    acel_x = dstBuff[1];
	    acel_x = (acel_x << 8) | dstBuff[2];
	    acel_y = dstBuff[3];
	    acel_y = (acel_y << 8) | dstBuff[4];
	    acel_z = dstBuff[5];
	    acel_z = (acel_z << 8) | dstBuff[6];


//	    acel_y = (dstBuff[2] << 8) | dstBuff[3];
//	    acel_z = (dstBuff[4] << 8) | dstBuff[5];
//	    temperature = (dstBuff[6] << 8) | dstBuff[7];
	    printf("%d\t\t%d\t\t%d\n",acel_x,acel_y,acel_z);
	    //printf("%d \n",acel_x);
	}
}
#if defined(LOGICALIS_DEBUG_IMU_ICM_20602_LIB) && (LOGICALIS_DEBUG_IMU_ICM_20602_LIB)


#endif // LOGICALIS_DEBUG_IMU_ICM_20602_LIB
