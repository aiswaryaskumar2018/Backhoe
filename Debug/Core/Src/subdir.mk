################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/AES.c \
../Core/Src/AirPressureGauge.c \
../Core/Src/BattVolt.c \
../Core/Src/Brightness.c \
../Core/Src/CAN_Tx.c \
../Core/Src/DefGauge.c \
../Core/Src/DieselParticulateFilter.c \
../Core/Src/DiscreteSwitch.c \
../Core/Src/EngineCoolTemp.c \
../Core/Src/EngineOilPress.c \
../Core/Src/FuelGauze.c \
../Core/Src/HourMeter.c \
../Core/Src/IICRoutine.c \
../Core/Src/J1939.c \
../Core/Src/MuxDemux.c \
../Core/Src/Odo.c \
../Core/Src/P15765.c \
../Core/Src/POPUPMessages.c \
../Core/Src/RpmMeter.c \
../Core/Src/Screen.c \
../Core/Src/SootLoad.c \
../Core/Src/Speedo.c \
../Core/Src/TML_14229_UDS.c \
../Core/Src/TellTales.c \
../Core/Src/TransOilPressreure.c \
../Core/Src/TransOilTemp.c \
../Core/Src/adc.c \
../Core/Src/curve.c \
../Core/Src/extEEPROM.c \
../Core/Src/ledDriver5716.c \
../Core/Src/main.c \
../Core/Src/stm32u5xx_hal_msp.c \
../Core/Src/stm32u5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u5xx.c 

C_DEPS += \
./Core/Src/AES.d \
./Core/Src/AirPressureGauge.d \
./Core/Src/BattVolt.d \
./Core/Src/Brightness.d \
./Core/Src/CAN_Tx.d \
./Core/Src/DefGauge.d \
./Core/Src/DieselParticulateFilter.d \
./Core/Src/DiscreteSwitch.d \
./Core/Src/EngineCoolTemp.d \
./Core/Src/EngineOilPress.d \
./Core/Src/FuelGauze.d \
./Core/Src/HourMeter.d \
./Core/Src/IICRoutine.d \
./Core/Src/J1939.d \
./Core/Src/MuxDemux.d \
./Core/Src/Odo.d \
./Core/Src/P15765.d \
./Core/Src/POPUPMessages.d \
./Core/Src/RpmMeter.d \
./Core/Src/Screen.d \
./Core/Src/SootLoad.d \
./Core/Src/Speedo.d \
./Core/Src/TML_14229_UDS.d \
./Core/Src/TellTales.d \
./Core/Src/TransOilPressreure.d \
./Core/Src/TransOilTemp.d \
./Core/Src/adc.d \
./Core/Src/curve.d \
./Core/Src/extEEPROM.d \
./Core/Src/ledDriver5716.d \
./Core/Src/main.d \
./Core/Src/stm32u5xx_hal_msp.d \
./Core/Src/stm32u5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u5xx.d 

OBJS += \
./Core/Src/AES.o \
./Core/Src/AirPressureGauge.o \
./Core/Src/BattVolt.o \
./Core/Src/Brightness.o \
./Core/Src/CAN_Tx.o \
./Core/Src/DefGauge.o \
./Core/Src/DieselParticulateFilter.o \
./Core/Src/DiscreteSwitch.o \
./Core/Src/EngineCoolTemp.o \
./Core/Src/EngineOilPress.o \
./Core/Src/FuelGauze.o \
./Core/Src/HourMeter.o \
./Core/Src/IICRoutine.o \
./Core/Src/J1939.o \
./Core/Src/MuxDemux.o \
./Core/Src/Odo.o \
./Core/Src/P15765.o \
./Core/Src/POPUPMessages.o \
./Core/Src/RpmMeter.o \
./Core/Src/Screen.o \
./Core/Src/SootLoad.o \
./Core/Src/Speedo.o \
./Core/Src/TML_14229_UDS.o \
./Core/Src/TellTales.o \
./Core/Src/TransOilPressreure.o \
./Core/Src/TransOilTemp.o \
./Core/Src/adc.o \
./Core/Src/curve.o \
./Core/Src/extEEPROM.o \
./Core/Src/ledDriver5716.o \
./Core/Src/main.o \
./Core/Src/stm32u5xx_hal_msp.o \
./Core/Src/stm32u5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u5xx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5G7xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/AES.cyclo ./Core/Src/AES.d ./Core/Src/AES.o ./Core/Src/AES.su ./Core/Src/AirPressureGauge.cyclo ./Core/Src/AirPressureGauge.d ./Core/Src/AirPressureGauge.o ./Core/Src/AirPressureGauge.su ./Core/Src/BattVolt.cyclo ./Core/Src/BattVolt.d ./Core/Src/BattVolt.o ./Core/Src/BattVolt.su ./Core/Src/Brightness.cyclo ./Core/Src/Brightness.d ./Core/Src/Brightness.o ./Core/Src/Brightness.su ./Core/Src/CAN_Tx.cyclo ./Core/Src/CAN_Tx.d ./Core/Src/CAN_Tx.o ./Core/Src/CAN_Tx.su ./Core/Src/DefGauge.cyclo ./Core/Src/DefGauge.d ./Core/Src/DefGauge.o ./Core/Src/DefGauge.su ./Core/Src/DieselParticulateFilter.cyclo ./Core/Src/DieselParticulateFilter.d ./Core/Src/DieselParticulateFilter.o ./Core/Src/DieselParticulateFilter.su ./Core/Src/DiscreteSwitch.cyclo ./Core/Src/DiscreteSwitch.d ./Core/Src/DiscreteSwitch.o ./Core/Src/DiscreteSwitch.su ./Core/Src/EngineCoolTemp.cyclo ./Core/Src/EngineCoolTemp.d ./Core/Src/EngineCoolTemp.o ./Core/Src/EngineCoolTemp.su ./Core/Src/EngineOilPress.cyclo ./Core/Src/EngineOilPress.d ./Core/Src/EngineOilPress.o ./Core/Src/EngineOilPress.su ./Core/Src/FuelGauze.cyclo ./Core/Src/FuelGauze.d ./Core/Src/FuelGauze.o ./Core/Src/FuelGauze.su ./Core/Src/HourMeter.cyclo ./Core/Src/HourMeter.d ./Core/Src/HourMeter.o ./Core/Src/HourMeter.su ./Core/Src/IICRoutine.cyclo ./Core/Src/IICRoutine.d ./Core/Src/IICRoutine.o ./Core/Src/IICRoutine.su ./Core/Src/J1939.cyclo ./Core/Src/J1939.d ./Core/Src/J1939.o ./Core/Src/J1939.su ./Core/Src/MuxDemux.cyclo ./Core/Src/MuxDemux.d ./Core/Src/MuxDemux.o ./Core/Src/MuxDemux.su ./Core/Src/Odo.cyclo ./Core/Src/Odo.d ./Core/Src/Odo.o ./Core/Src/Odo.su ./Core/Src/P15765.cyclo ./Core/Src/P15765.d ./Core/Src/P15765.o ./Core/Src/P15765.su ./Core/Src/POPUPMessages.cyclo ./Core/Src/POPUPMessages.d ./Core/Src/POPUPMessages.o ./Core/Src/POPUPMessages.su ./Core/Src/RpmMeter.cyclo ./Core/Src/RpmMeter.d ./Core/Src/RpmMeter.o ./Core/Src/RpmMeter.su ./Core/Src/Screen.cyclo ./Core/Src/Screen.d ./Core/Src/Screen.o ./Core/Src/Screen.su ./Core/Src/SootLoad.cyclo ./Core/Src/SootLoad.d ./Core/Src/SootLoad.o ./Core/Src/SootLoad.su ./Core/Src/Speedo.cyclo ./Core/Src/Speedo.d ./Core/Src/Speedo.o ./Core/Src/Speedo.su ./Core/Src/TML_14229_UDS.cyclo ./Core/Src/TML_14229_UDS.d ./Core/Src/TML_14229_UDS.o ./Core/Src/TML_14229_UDS.su ./Core/Src/TellTales.cyclo ./Core/Src/TellTales.d ./Core/Src/TellTales.o ./Core/Src/TellTales.su ./Core/Src/TransOilPressreure.cyclo ./Core/Src/TransOilPressreure.d ./Core/Src/TransOilPressreure.o ./Core/Src/TransOilPressreure.su ./Core/Src/TransOilTemp.cyclo ./Core/Src/TransOilTemp.d ./Core/Src/TransOilTemp.o ./Core/Src/TransOilTemp.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/curve.cyclo ./Core/Src/curve.d ./Core/Src/curve.o ./Core/Src/curve.su ./Core/Src/extEEPROM.cyclo ./Core/Src/extEEPROM.d ./Core/Src/extEEPROM.o ./Core/Src/extEEPROM.su ./Core/Src/ledDriver5716.cyclo ./Core/Src/ledDriver5716.d ./Core/Src/ledDriver5716.o ./Core/Src/ledDriver5716.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32u5xx_hal_msp.cyclo ./Core/Src/stm32u5xx_hal_msp.d ./Core/Src/stm32u5xx_hal_msp.o ./Core/Src/stm32u5xx_hal_msp.su ./Core/Src/stm32u5xx_it.cyclo ./Core/Src/stm32u5xx_it.d ./Core/Src/stm32u5xx_it.o ./Core/Src/stm32u5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u5xx.cyclo ./Core/Src/system_stm32u5xx.d ./Core/Src/system_stm32u5xx.o ./Core/Src/system_stm32u5xx.su

.PHONY: clean-Core-2f-Src

