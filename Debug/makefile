################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/common/gatt_db/subdir.mk
-include source/common/subdir.mk
-include source/Logicalis_HAL/subdir.mk
-include source/subdir.mk
-include framework/common/subdir.mk
-include framework/TimersManager/Source/subdir.mk
-include framework/Shell/Source/subdir.mk
-include framework/SerialManager/Source/UART_Adapter/subdir.mk
-include framework/SerialManager/Source/SPI_Adapter/subdir.mk
-include framework/SerialManager/Source/I2C_Adapter/subdir.mk
-include framework/SerialManager/Source/subdir.mk
-include framework/SecLib/subdir.mk
-include framework/Reset/subdir.mk
-include framework/RNG/Source/subdir.mk
-include framework/Panic/Source/subdir.mk
-include framework/OSAbstraction/Source/subdir.mk
-include framework/NVM/Source/subdir.mk
-include framework/ModuleInfo/subdir.mk
-include framework/Messaging/Source/subdir.mk
-include framework/MemManager/Source/subdir.mk
-include framework/MWSCoexistence/Source/subdir.mk
-include framework/Lists/subdir.mk
-include framework/LED/Source/subdir.mk
-include framework/Keyboard/Source/subdir.mk
-include framework/GPIO/subdir.mk
-include framework/FunctionLib/subdir.mk
-include framework/Flash/Internal/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include bluetooth/profiles/temperature/subdir.mk
-include bluetooth/profiles/device_info/subdir.mk
-include bluetooth/profiles/battery/subdir.mk
-include bluetooth/host/config/subdir.mk
-include bluetooth/hci_transport/source/subdir.mk
-include bluetooth/controller/interface/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf

# Tool invocations
qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -L"C:\Users\re91535z\Documents\MCUXpressoIDE_10.1.1_606\workspace\qn908xcdk_wireless_examples_bluetooth_ble_shell_bm\libs" -Xlinker --defsym=gUseNVMLink_d=1 -Xlinker --no-wchar-size-warning -Xlinker -print-memory-usage -Xlinker --defsym=__ram_vector_table__=1 -Xlinker --gc-sections -Xlinker -Map="qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.map" -Xlinker --defsym=gEraseNVMLink_d=1 -Xlinker --just-symbols="../board/fw_symbols_gcc.h" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T QN908XC_connectivity.ld -L ../source -o "qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf"; # arm-none-eabi-objcopy -v -O binary "qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.axf" "qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.bin" ; # checksum -p QN908XC -d "qn908xcdk_wireless_examples_bluetooth_ble_shell_bm.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
