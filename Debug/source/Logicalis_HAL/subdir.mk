################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Logicalis_HAL/aes.c \
../source/Logicalis_HAL/clock_QN9080.c \
../source/Logicalis_HAL/delay.c \
../source/Logicalis_HAL/driver_icm20602.c \
../source/Logicalis_HAL/imu_icm_20602.c \
../source/Logicalis_HAL/input_pin.c \
../source/Logicalis_HAL/log_uart.c \
../source/Logicalis_HAL/mcu_watchdog.c \
../source/Logicalis_HAL/memory_lib.c \
../source/Logicalis_HAL/mx25r_flash.c \
../source/Logicalis_HAL/ring_buffer.c \
../source/Logicalis_HAL/rtc_lib.c \
../source/Logicalis_HAL/serial_flexcomm.c \
../source/Logicalis_HAL/spi_lib.c \
../source/Logicalis_HAL/string_tools.c 

OBJS += \
./source/Logicalis_HAL/aes.o \
./source/Logicalis_HAL/clock_QN9080.o \
./source/Logicalis_HAL/delay.o \
./source/Logicalis_HAL/driver_icm20602.o \
./source/Logicalis_HAL/imu_icm_20602.o \
./source/Logicalis_HAL/input_pin.o \
./source/Logicalis_HAL/log_uart.o \
./source/Logicalis_HAL/mcu_watchdog.o \
./source/Logicalis_HAL/memory_lib.o \
./source/Logicalis_HAL/mx25r_flash.o \
./source/Logicalis_HAL/ring_buffer.o \
./source/Logicalis_HAL/rtc_lib.o \
./source/Logicalis_HAL/serial_flexcomm.o \
./source/Logicalis_HAL/spi_lib.o \
./source/Logicalis_HAL/string_tools.o 

C_DEPS += \
./source/Logicalis_HAL/aes.d \
./source/Logicalis_HAL/clock_QN9080.d \
./source/Logicalis_HAL/delay.d \
./source/Logicalis_HAL/driver_icm20602.d \
./source/Logicalis_HAL/imu_icm_20602.d \
./source/Logicalis_HAL/input_pin.d \
./source/Logicalis_HAL/log_uart.d \
./source/Logicalis_HAL/mcu_watchdog.d \
./source/Logicalis_HAL/memory_lib.d \
./source/Logicalis_HAL/mx25r_flash.d \
./source/Logicalis_HAL/ring_buffer.d \
./source/Logicalis_HAL/rtc_lib.d \
./source/Logicalis_HAL/serial_flexcomm.d \
./source/Logicalis_HAL/spi_lib.d \
./source/Logicalis_HAL/string_tools.d 


# Each subdirectory must supply rules for building sources it contributes
source/Logicalis_HAL/%.o: ../source/Logicalis_HAL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=1 -DCPU_QN908X=1 -DDEBUG -DCFG_BLE_PRJ=1 -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DCPU_QN9080C -DCPU_QN9080C_cm4 -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\board" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\Logicalis_HAL" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\OSAbstraction\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\common" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Flash\Internal" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\GPIO" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Keyboard\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\TimersManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\TimersManager\Source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\LED\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\I2C_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\SPI_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SerialManager\Source\UART_Adapter" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\MemManager\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Lists" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Messaging\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Panic\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\RNG\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\NVM\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\NVM\Source" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\Shell\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\ModuleInfo" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\FunctionLib" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\SecLib" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\host\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\common" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\host\config" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\controller\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\hci_transport\interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\source\common\gatt_db" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\battery" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\device_info" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\bluetooth\profiles\temperature" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\framework\MWSCoexistence\Interface" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\drivers" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\utilities" -I"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\CMSIS" -O0 -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -imacros "C:/Users/re91535z/Documents/MCUXpressoIDE_10.1.1_606/workspace/qn908xcdk_wireless_examples_bluetooth_ble_shell_bm/source/app_preinclude.h" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


