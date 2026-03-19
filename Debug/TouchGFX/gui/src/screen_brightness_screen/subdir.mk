################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.cpp \
../TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.cpp 

OBJS += \
./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.o \
./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.o 

CPP_DEPS += \
./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.d \
./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/screen_brightness_screen/%.o TouchGFX/gui/src/screen_brightness_screen/%.su TouchGFX/gui/src/screen_brightness_screen/%.cyclo: ../TouchGFX/gui/src/screen_brightness_screen/%.cpp TouchGFX/gui/src/screen_brightness_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m33 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5G7xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-gui-2f-src-2f-screen_brightness_screen

clean-TouchGFX-2f-gui-2f-src-2f-screen_brightness_screen:
	-$(RM) ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.cyclo ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.d ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.o ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessPresenter.su ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.cyclo ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.d ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.o ./TouchGFX/gui/src/screen_brightness_screen/Screen_brightnessView.su

.PHONY: clean-TouchGFX-2f-gui-2f-src-2f-screen_brightness_screen

