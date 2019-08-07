/*! *********************************************************************************
* Copyright 2018 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#include "fsl_common.h"
#include "clock_config.h" /* for BOARD_XTAL0_CLK_HZ */

#if (defined(BOARD_XTAL1_CLK_HZ) && (BOARD_XTAL1_CLK_HZ != CLK_XTAL_32KHZ))

#include "fsl_ctimer.h"
#include "rco32k_calibration.h"
#include "fsl_rtc.h"
#include "TimersManager.h"
#if (cPWR_UsePowerDownMode)
#include "PWR_Interface.h"
#include "PWR_Configuration.h"
#include "controller_interface.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CFG_WAIT_CAPTURE_READY_TIMES        0
#define CFG_CAPTURE_COUNT_STEP_LIMIT        5
#define CFG_CAPTURE_INTER_DISTURBED_LIMIT   50

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void RCO32K_CfgCtimer(void *param);
static void RCO32K_CaptureEdgeCb(uint32_t flag);
void RCO32K_UpdateSwCalibInterval(uint32_t interval);
void set_32k_ppm(int32_t ppm);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static rco32k_calib_env_t s_rco32kEnv = {
    .ctimer     = CTIMER3, /* use CTIMER3 for RCO32K's software calibration by default */
    .ctimer_irq = CTIMER3_IRQn,
    .interval   = 0U,    /* RCO32K software calibration interval 0ms */
    .hwCodeFlag = 0,
    .total_calibrations_executed = 0,
};

/* when using kCTIMER_SingleCallback, this array has only one element */
static ctimer_callback_t s_CtimerCbArray[] = {
    RCO32K_CaptureEdgeCb,
};

/* record the lost interrupt count */
static uint32_t s_intr_cnt = 0;
/* calibration count, enable filter policy when it is bigger than CFG_WAIT_CAPTURE_READY_TIMES */
static uint8_t s_cap_times = 0;
static uint8_t s_calibration_started = 0;

static tmrTimerID_t s_rco32KTimerID = gTmrInvalidTimerID_c;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Calculate the ppm value of RCO32K and a) set it to RTC calibration register,
 *  b) pass it to ble stack.
 *
 *  ppm = (0x100000ull * 32000(Hz) * cnt) / (apb_clock(Hz) * ncycle(32K cycle count))
 * If apb clock is  8MHz, ppm = (0x8000 / 1) * cnt / 125
 * If apb clock is 16MHz, ppm = (0x8000 / 2) * cnt / 125
 * If apb clock is 32MHz, ppm = (0x8000 / 4) * cnt / 125
 *
 * @param cnt  delta of ctimer counter value during actual 16 32k cycles.
 *             During 16 precise 32k cycles(500us), if APB is 16M, cnt equals 8000.
 */
static inline void RCO32K_CalcPpm(uint32_t cnt)
{
    int32_t dir      = 0;
    int32_t real_ppm = 0;
    uint32_t ppm;
    uint32_t tmp;

    tmp = (CLOCK_GetFreq(kCLOCK_ApbClk) / 8000000U);
    tmp = 0x8000U / tmp * cnt;

    /* 1/125 ~= 2^(-7) + 2^(-13) + 2^(-14) + 2^(-18) + 2^(-21) + 2^(-24) + 2^(-25) + 2^(-26)
     * deviation: ~1.3e-9 */
    ppm = (tmp >> 7U) + (tmp >> 13U) + (tmp >> 14U) + (tmp >> 18U) + (tmp >> 21U) + (tmp >> 24U) + (tmp >> 25U) +
          (tmp >> 26U);

    if (ppm > 0x100000U)
    {
        ppm = ppm - 0x100000U;
        dir = -1;
    }
    else
    {
        ppm = 0x100000U - ppm;
        dir = 1;
    }
    
    if (ppm > 0xA000U)
    {
        s_rco32kEnv.hwCodeFlag = dir;
    }
    
    RTC_Calibration(RTC, (1 == dir) ? kRTC_BackwardCalibration : kRTC_ForwardCalibration, ppm); /* write to rtc calibration register */
    TMR_NotifyClkChanged(); // Notify TimersManager about the real frequency
    real_ppm = (dir)*(-1)*ppm;
    set_32k_ppm(real_ppm);  // Notify BLE controller about the real frequency
}


/*!
 * @brief Deal with 32k clock capture interrupt of ctimer.
 *
 * 32k's rise edge interrupt happens every ~31.25us, if any critical section blocks the interrupt
 * for long period, interrupt may get lost. So if delta of current capture value and last capture
 * value is multiple of ~31.25us(250 apb ticks in case of 8M apb), we need to compensate software
 * capture counter. And what's more, the accuracy of 32k rco is +/-6%, worst case value
 * 250 * (1 - 6%) = 235 is used as divisor.
 *
 * @param flag Interrupt flag of ctimer.
 */
static void RCO32K_CaptureEdgeCb(uint32_t flag)
{
    uint32_t cap = 0U;
    uint32_t delta = 0U;
    uint32_t cnt = 0U;
    bool calibration_end = false;
            
    if (flag & CTIMER_IR_CR2INT_MASK)
    {
        cap = s_rco32kEnv.ctimer->CR[kCTIMER_Capture_2];
        s_rco32kEnv.cnt++;

        if (s_rco32kEnv.cnt == RCO32K_1ST_EDGE)
        {
            s_rco32kEnv.firstVal = cap;
        }
        else
        {
            uint32_t cap_cnt = 0U;

            /* compensate software capture counter */
            delta = cap - s_rco32kEnv.lastVal;

            cap_cnt = s_rco32kEnv.cnt - 1;
            s_rco32kEnv.cnt += ((delta / (235U * (CLOCK_GetFreq(kCLOCK_ApbClk) / 8000000U))) - 1U);
            cap_cnt = s_rco32kEnv.cnt - cap_cnt;

            // Don't check this limit at first time
            if ((s_cap_times > CFG_WAIT_CAPTURE_READY_TIMES) && (cap_cnt > CFG_CAPTURE_COUNT_STEP_LIMIT))
            {
                s_intr_cnt++;
                if (s_intr_cnt > CFG_CAPTURE_INTER_DISTURBED_LIMIT)
                {
                    calibration_end = true;
                }
                else
                {   //calibration will be restarted from 1st edge
                    s_rco32kEnv.firstVal = 0U;
                    s_rco32kEnv.cnt = 0U;
                    return;
                }
            } 
            
            if (s_rco32kEnv.cnt >= RCO32K_17TH_EDGE)
            {  
                /* pass apb cycle count during 16 32k cycles to RCO32K_CalcPpm() */
                delta = cap - s_rco32kEnv.firstVal;
                cnt = delta - ((s_rco32kEnv.cnt - RCO32K_17TH_EDGE) * delta / (s_rco32kEnv.cnt - 1U));

                RCO32K_CalcPpm(cnt);

                if (s_cap_times <= CFG_WAIT_CAPTURE_READY_TIMES)
                    s_cap_times++;

                calibration_end = true;
            }

            if (calibration_end)
            {
                s_intr_cnt = 0;
                s_rco32kEnv.ctimer->CCR &= ~(CTIMER_CCR_CAP2RE_MASK | CTIMER_CCR_CAP2FE_MASK | CTIMER_CCR_CAP2I_MASK);

                if (CTIMER_GetStatusFlags(s_rco32kEnv.ctimer) & CTIMER_IR_CR2INT_MASK)
                {
                    CTIMER_ClearStatusFlags(s_rco32kEnv.ctimer, CTIMER_IR_CR2INT_MASK);
                }
               
                CTIMER_Deinit(s_rco32kEnv.ctimer);
                NVIC_ClearPendingIRQ((IRQn_Type)s_rco32kEnv.ctimer_irq);
                
                s_rco32kEnv.total_calibrations_executed++;
                s_calibration_started = 0;
                
                #if (cPWR_UsePowerDownMode)
                BLE_enable_sleep();  // Allow device to sleep
                #endif
            }
        }
        s_rco32kEnv.lastVal = cap;
    }
}


/*!
 * @brief Configure ctimer to capture RCO32K waveform's rising edge.
 *
 * Ctimer uses APB as clock source, to capture 17 32k clock's rising edge.
 * The capture interrupt happens every 32k rise edge (~31.25us).
 *
 * @param none
 */
static void RCO32K_CfgCtimer(void *param)
{
    ctimer_config_t config;
    uint32_t code;
    
    if (s_calibration_started != 0)
    {
        return; //Calibration already started but not yet finished, do not interrupt!
    }
    
    s_calibration_started = 1;
                
    if (s_rco32kEnv.hwCodeFlag != 0)
    {
        code = CALIB->RCO_RC_REF_OSC_CFG & CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_MASK;
        
        if (s_rco32kEnv.hwCodeFlag == 1)
        {
            code++;
        }
        else
        {
            code--;
        }

        CALIB->RCO_RC_REF_OSC_CFG = (CALIB->RCO_RC_REF_OSC_CFG & ~CALIB_RCO_RC_REF_OSC_CFG_CAU_RCO_CAP_CFG_MASK) | code;
        s_rco32kEnv.hwCodeFlag = 0;
    }

    s_rco32kEnv.firstVal= 0U;
    s_rco32kEnv.cnt     = 0U;

    config.mode         = kCTIMER_TimerMode;
    config.input        = kCTIMER_Capture_2;
    config.prescale     = 0U;

    CTIMER_Init(s_rco32kEnv.ctimer, &config);
    CTIMER_StartTimer(s_rco32kEnv.ctimer);
    CTIMER_RegisterCallBack(s_rco32kEnv.ctimer, s_CtimerCbArray, kCTIMER_SingleCallback);
    CTIMER_SetupCapture(s_rco32kEnv.ctimer, kCTIMER_Capture_2, kCTIMER_Capture_RiseEdge, true);
    
    if (3 == s_rco32kEnv.total_calibrations_executed)
    { //this is the 3rd calibration after power-up, finish this and then obey the s_rco32kEnv.interval calibration interval
        RCO32K_UpdateSwCalibInterval(s_rco32kEnv.interval);
    }
                
#if (cPWR_UsePowerDownMode)
    BLE_disable_sleep(); //Disallow device to sleep
#endif
}

void RCO32K_InitSwCalib(uint32_t interval)
{
    s_rco32kEnv.interval = interval;
    
    if (s_rco32kEnv.interval)
    {
        TMR_Init(); //Initialize TimersManager, if already initialized this call will do nothing.
        
        if(s_rco32KTimerID == gTmrInvalidTimerID_c)
        {
            s_rco32KTimerID = TMR_AllocateTimer();
            TMR_StartLowPowerTimer(s_rco32KTimerID, gTmrLowPowerIntervalMillisTimer_c, 300, RCO32K_CfgCtimer, NULL); //execute the first 3 calibrations at 300ms intervals, then obey s_rco32kEnv.interval
        }
        else
        {
            TMR_StartLowPowerTimer(s_rco32KTimerID, gTmrLowPowerIntervalMillisTimer_c, s_rco32kEnv.interval, RCO32K_CfgCtimer, NULL);
        }
    }
}

/* Updates the s_rco32kEnv.interval 
   If input is zero the s_rco32KTimer is stopped. */
void RCO32K_UpdateSwCalibInterval(uint32_t interval)
{
    if(0 == interval)
    {
      /* stop the timer */
      TMR_StopTimer(s_rco32KTimerID);
      
      /* reset calibration variables to their default values */
      s_rco32kEnv.interval = 0;
      s_rco32kEnv.hwCodeFlag = 0;
      s_rco32kEnv.total_calibrations_executed = 0;      
      s_intr_cnt = 0;
      s_cap_times = 0;
      s_calibration_started = 0;
      
      return;
    }
    
    s_rco32kEnv.interval = interval;
    
    if (s_rco32kEnv.interval)
    {
        TMR_StartLowPowerTimer(s_rco32KTimerID, gTmrLowPowerIntervalMillisTimer_c, s_rco32kEnv.interval, RCO32K_CfgCtimer, NULL);
    }
}
#endif
