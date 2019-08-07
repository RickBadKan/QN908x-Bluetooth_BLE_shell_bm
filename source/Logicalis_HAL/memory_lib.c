/**
\file    memory_lib.h
\brief   Responsável pelas leituras e escritas em uma memória.

\details
         Funcionamento da flash (From MX25R2035F):
		    1.	Check status register to ensure device is ready
			

\b@{Histórico de Alterações:@}
    - 2019.05.08 -- Primeira versão (v1.0.0)
    
\author
  Renato Zapata Lusni

*/
//==============================================================================
//
// Includes
//
//==============================================================================
#include "fsl_spi.h"
#include "mx25r_flash.h"
#include "memory_lib.h"
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
static spi_master_handle_t g_handle;

struct mx25r_instance mx25r;

// NOTE: Foi testado colocar a definição da variável abaixo no .h, isto gera um erro
//       de link! make: *** Error 1
volatile bool transfer_is_done = false;

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
int flash_transfer_cb(void *transfer_prv, uint8_t *tx_data, uint8_t *rx_data, uint32_t dataSize, bool eof)
{
    spi_transfer_t xfer = {0};
    xfer.txData = tx_data;
    xfer.rxData = rx_data;
    xfer.dataSize = dataSize;
    /* terminate frame */
    if (eof)
    {
        xfer.configFlags |= kSPI_FrameAssert;
    }

    transfer_is_done = false;
    /* transfer nonblocking */
    SPI_MasterTransferBlocking((SPI_Type *)transfer_prv, &xfer);
    /* until transfer ends */
//    while (!transfer_is_done)
//    {
//    }

    return 0;
}



/**
 * @brief
 *
 * @param
 *
 * @return
 */
void masterCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    transfer_is_done = true;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
int memory_init(SPI_Type *base)
{
	spi_master_config_t masterConfig = {0};
    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.direction = kSPI_MsbFirst;
    masterConfig.polarity = kSPI_ClockPolarityActiveHigh;
    masterConfig.phase = kSPI_ClockPhaseFirstEdge;
    masterConfig.baudRate_Bps = 100000;
    masterConfig.sselNum = (spi_ssel_t)FLASH_SPI_SSEL;
    masterConfig.sselPol = (spi_spol_t)EXAMPLE_SPI_SPOL;
    SPI_MasterInit(base, &masterConfig, EXAMPLE_SPI_MASTER_CLK_FREQ);
    SPI_MasterTransferCreateHandle(base, &g_handle, masterCallback, NULL);
    mx25r_init(&mx25r, flash_transfer_cb, base);
    return mx25r_err_ok;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
mx25r_err_t memory_write(uint32_t address, char *buffer, char size)
{
	//=================================================
	//     ESCREVE O BUFFER NA MEMÓRIA
	//=================================================
    /* check sector alignement */
    int status = (address % 2048) ? mx25r_err_alignement : mx25r_err_ok;
    if (mx25r_err_ok != status)
    {
        printf("Endereço nao alinhado a pagina");
    }

    /* write command */
    status = mx25r_cmd_write(&mx25r, address, (uint8_t *)buffer, size);
    if (mx25r_err_ok != status)
    {
    	// TODO: Decidir se coloca printf ou log_uart nesta parte
        printf("'mx25r_cmd_write' failed %d\r\n ",status);
    }
    return status;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
mx25r_err_t memory_read(uint32_t address, char *buffer, char size)
{
	//=================================================
	//     LÊ BUFFER NO SETOR 0
	//=================================================
    /* check sector alignement */
    int status = (address % SECTOR_SIZE) ? mx25r_err_alignement : mx25r_err_ok;
//    if (mx25r_err_ok != status)
//    {
//        return status;
//    }

    /* read command */
    status = mx25r_cmd_read(&mx25r, address, (uint8_t *)buffer, size);

    if (mx25r_err_ok != status)
    {
        printf("'mx25r_cmd_read' failed \r\n");
    }
    return status;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
int memory_erase_sector(uint32_t address)
{
    /* check sector alignement */
    int status = (address % SECTOR_SIZE) ? mx25r_err_alignement : mx25r_err_ok;
    if (mx25r_err_ok != status)
    {
        return status;
    }
    /* erase command */
    status = mx25r_cmd_sector_erase(&mx25r, address);
    if (mx25r_err_ok != status)
    {
        printf("'mx25r_cmd_sector_erase' failed \r\n");
    }
    return status;
}


/**
 * @brief Coloca a memória em modo de baixa energia.
 *
 * @param pwrDwnConfig: Configuração do powerdown.
 *
 * @return
 */
int enterPowerDown(flash_powerdown_config_t *pwrDwnConfig)
{

	// Set the flag	
	pwrDwnConfig->lowPwr=1;

	// From MX25R2035F:	

	// CS# goes low.
	
	// Send DP instruction code.
	
	// CS# goes high.
	
	// Wait tDP - transient - for safety.

	// Todos os passo acima estão implementados aqui:
	mx25r_cmd_dp(&mx25r);

	return 0;
}

/**
 * @brief Faz uma leitura do status register para acordar o dispositivo se necessário.
 *
 * @param pwrDwnConfig: Configuração do powerdown.
 *
 * @return
 */
mx25r_err_t enterStandBy(flash_powerdown_config_t *pwrDwnConfig)
{

	// É necessario esperar o tempo de tDPDD, igual a 30us

	struct mx25r_rdsr_result resultado;


	mx25r_err_t return_error = mx25r_cmd_rdsr(&mx25r,&resultado);
	if (return_error!=mx25r_err_ok)
	{

		printf("Erro ao enviar comando de leitura de status register durante wakeup process");
	}
	else
	{
		printf("Sucesso ao acordar o device: %x",resultado);
	}
	// Set the flag 
	pwrDwnConfig->lowPwr=0;

	// Guarantee the minimum tDPDD has passed.
	

	// CS# pulses low for tCRDP.
	

	// wait tRDP for it to transit from power down to stand-by.
	return return_error;
}

 
#if defined(LOGICALIS_DEBUG_MEMORY_LIB) && (LOGICALIS_DEBUG_MEMORY_LIB)
//TODO: Criar um teste variando o tamanho do bloco.
	// Rotina genérica de delay:

#include <string.h>


	/**
	 * @brief Verifica o status da memória, lê e espera a leitura terminar
	 *
	 * @param
	 * 			address: Endereço inicial de leitura.
	 * 			buffer: Buffer onde a leitura será salva.
	 * 			size: Tamanho do buffer de leitura.
	 *
	 * @return
	 */
	static void read(uint32_t address, char *buffer, char size)
	{
		int status;
	    /* Wait while WIP is busy */

		struct mx25r_rdsr_result result;

		// Checa se a memória está ocupada
	    do
	    {
	        mx25r_cmd_rdsr(&mx25r, &result);
	    } while (result.sr0 & 0x1);

	    // Faz a leitura
		status = memory_read(address,buffer,size);
		if (mx25r_err_ok != status)
		{
			printf("\nErro ao ler");
		}

		// Checa se a memória ainda está ocupada
	    do
	    {
	        mx25r_cmd_rdsr(&mx25r, &result);
	    } while (result.sr0 & 0x1);


	}

	/**
	 * @brief Verifica o status da memória, escreve e espera a escrita terminar.
	 *
	 * @param
	 * 			address: Endereço inicial de escrita.
	 * 			buffer: Buffer que será salva.
	 * 			size: Tamanho do buffer de escrita.
	 *
	 * @return
	 */
	static void write(uint32_t address, char *buffer, char size)
	{
		int status;
	    /* Wait while WIP is busy */

		struct mx25r_rdsr_result result;

		// Checa se a memória está ocupada
	    do
	    {
	        mx25r_cmd_rdsr(&mx25r, &result);
	    } while (result.sr0 & 0x1);

	    // Faz a leitura
		status = memory_write(address,buffer,size);
		if (mx25r_err_ok != status)
		{
			printf("\nErro ao ler");
		}

		// Checa se a memória ainda está ocupada
	    do
	    {
	        mx25r_cmd_rdsr(&mx25r, &result);
	    } while (result.sr0 & 0x1);


	}
	void memory_lib_debug_write_read(uint32_t setor_tested)
	{
		//=================================================
		//     FLASH INIT
		//=================================================

		if (mx25r_err_ok != memory_init(SPI0))
		{
			printf("\nErro ao iniciar memoria");
		}

		// Escrita em um setor em blocos de 255 bytes, faz a leitura do bloco e segue para o próximo.
		//
		if (setor_tested>=NUMBER_OF_SECTORS)
		{
			printf("\n Numero do setor inválido");
		}

		// Pega o valor do setor:
		uint32_t sector_address = setor_tested*4096;

		int status;

        status = memory_erase_sector(sector_address);
        if (mx25r_err_ok != status)
        {
        	printf("\n Erro ao apagar setor");
        }

        // Inicia um buffer para receber as leituras
        uint32_t buffer_size = 255;
        char read_buffer[buffer_size];

        uint32_t var=1;

        // Escrever dados em blocos de 256 alternadamente.
        for (int u=0;u<4096;u=u+512)
        {
        	// Inicia o buffer com 1s, desta forma, sabemos se foi gravado um valor ou não
            for (int i=0;i<buffer_size;i=i+1)
            {
            	read_buffer[i]=var;
            }
            var=var+1;
        	write(u, read_buffer, buffer_size);
        }

        for (int i=0;i<4096;i=i+256)
        {
        	printf("\n\n ============================== \n",i);
        	printf("\t\t Endereco %d \n",i);


        	read(i, read_buffer, buffer_size);
        	for (int v=0;v<buffer_size;v=v+1)
        	{
        		printf("\n address %d:\t %d",v+i,read_buffer[v]);
        	}
        }

		// Apaga o setor inteiro e lê todos

        // 10 caracteres

		// 63 caracteres

		// 500 caracteres

		// 1000
	}

//TODO: Criar teste para indicar que quando a flash está em low power, os comandos são ignorado. 
#endif // LOGICALIS_DEBUG_MEMORY_LIB
