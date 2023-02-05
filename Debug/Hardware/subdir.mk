################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/fonts.c \
../Hardware/gy30.c \
../Hardware/ssd1306.c 

OBJS += \
./Hardware/fonts.o \
./Hardware/gy30.o \
./Hardware/ssd1306.o 

C_DEPS += \
./Hardware/fonts.d \
./Hardware/gy30.d \
./Hardware/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/%.o Hardware/%.su: ../Hardware/%.c Hardware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L010xB -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../Core/System -I../Hardware -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Hardware

clean-Hardware:
	-$(RM) ./Hardware/fonts.d ./Hardware/fonts.o ./Hardware/fonts.su ./Hardware/gy30.d ./Hardware/gy30.o ./Hardware/gy30.su ./Hardware/ssd1306.d ./Hardware/ssd1306.o ./Hardware/ssd1306.su

.PHONY: clean-Hardware

