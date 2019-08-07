/**
\file    imu_icm_20602.c
\brief   Responsável por iniciar a IMU via SPI, fazer as configurações necessárias, enviar e receber.

\details 


\b@{Histórico de Alterações:@}
    - 2019.06.07 -- Primeira versão (v1.0.0)

\author
  Renato Souza

*/

#ifndef IMU_ICM_20602_LIB_H_
#define IMU_ICM_20602_LIB_H_



#define LOGICALIS_DEBUG_IMU_ICM_20602_LIB 1





//==============================================================================
//
// Variáveis globais
//
//==============================================================================
#define ACCEL_FCHOICE_B_SHIFT 	3	//Used to bypass DLPF
#define DEC2_CFG_SHIFT 			4	//Averaging filter settings for Low Power Accelerometer mode
#define ACCEL_FS_SEL_SHIFT 		3	//Accel Full Scale Select:
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
void imu_icm_20602_init(void);
void imu_icm_20602_read_loop(void);

#endif // LOGICALIS_DEBUG_SPI_LIB
#if defined(LOGICALIS_DEBUG_IMU_ICM_20602_LIB) && (LOGICALIS_DEBUG_IMU_ICM_20602_LIB)

#endif /* IMU_ICM_20602_LIB_H_ */


