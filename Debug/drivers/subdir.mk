################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc.c \
../drivers/fsl_aes.c \
../drivers/fsl_calibration.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_ctimer.c \
../drivers/fsl_flash.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_pint.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_rf.c \
../drivers/fsl_rng.c \
../drivers/fsl_rtc.c \
../drivers/fsl_spi.c \
../drivers/fsl_usart.c \
../drivers/fsl_wdt.c 

OBJS += \
./drivers/fsl_adc.o \
./drivers/fsl_aes.o \
./drivers/fsl_calibration.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_ctimer.o \
./drivers/fsl_flash.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_pint.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_rf.o \
./drivers/fsl_rng.o \
./drivers/fsl_rtc.o \
./drivers/fsl_spi.o \
./drivers/fsl_usart.o \
./drivers/fsl_wdt.o 

C_DEPS += \
./drivers/fsl_adc.d \
./drivers/fsl_aes.d \
./drivers/fsl_calibration.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_ctimer.d \
./drivers/fsl_flash.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_pint.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_rf.d \
./drivers/fsl_rng.d \
./drivers/fsl_rtc.d \
./drivers/fsl_spi.d \
./drivers/fsl_usart.d \
./drivers/fsl_wdt.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=1 -DCPU_QN908X=1 -DDEBUG -DCFG_BLE_PRJ=1 -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DCPU_QN9080C -DCPU_QN9080C_cm4 -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\board" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\Logicalis_HAL" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\OSAbstraction\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\common" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Flash\Internal" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\GPIO" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Keyboard\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\TimersManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\TimersManager\Source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\LED\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\I2C_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\SPI_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\UART_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\MemManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Lists" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Messaging\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Panic\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\RNG\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\NVM\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\NVM\Source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Shell\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\ModuleInfo" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\FunctionLib" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SecLib" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\host\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\common" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\host\config" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\controller\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\hci_transport\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\common\gatt_db" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\battery" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\device_info" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\temperature" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\MWSCoexistence\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\drivers" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\utilities" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\CMSIS" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -imacros "C:/Users/re91535z/Documents/MCUXpressoIDE_10.1.1_606/workspace/qn908xcdk_wireless_examples_bluetooth_ble_shell_bm/source/app_preinclude.h" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


