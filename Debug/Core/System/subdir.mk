################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/System/delay.c \
../Core/System/device_tree.c \
../Core/System/easy_log.c \
../Core/System/type_to_ascii.c 

OBJS += \
./Core/System/delay.o \
./Core/System/device_tree.o \
./Core/System/easy_log.o \
./Core/System/type_to_ascii.o 

C_DEPS += \
./Core/System/delay.d \
./Core/System/device_tree.d \
./Core/System/easy_log.d \
./Core/System/type_to_ascii.d 


# Each subdirectory must supply rules for building sources it contributes
Core/System/%.o Core/System/%.su: ../Core/System/%.c Core/System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L010xB -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../Core/System -I../Hardware -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-System

clean-Core-2f-System:
	-$(RM) ./Core/System/delay.d ./Core/System/delay.o ./Core/System/delay.su ./Core/System/device_tree.d ./Core/System/device_tree.o ./Core/System/device_tree.su ./Core/System/easy_log.d ./Core/System/easy_log.o ./Core/System/easy_log.su ./Core/System/type_to_ascii.d ./Core/System/type_to_ascii.o ./Core/System/type_to_ascii.su

.PHONY: clean-Core-2f-System

