################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/System/Drivers/Simulation/SI2C.c 

OBJS += \
./Core/System/Drivers/Simulation/SI2C.o 

C_DEPS += \
./Core/System/Drivers/Simulation/SI2C.d 


# Each subdirectory must supply rules for building sources it contributes
Core/System/Drivers/Simulation/%.o Core/System/Drivers/Simulation/%.su: ../Core/System/Drivers/Simulation/%.c Core/System/Drivers/Simulation/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L010xB -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../Core/System -I../Hardware -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-System-2f-Drivers-2f-Simulation

clean-Core-2f-System-2f-Drivers-2f-Simulation:
	-$(RM) ./Core/System/Drivers/Simulation/SI2C.d ./Core/System/Drivers/Simulation/SI2C.o ./Core/System/Drivers/Simulation/SI2C.su

.PHONY: clean-Core-2f-System-2f-Drivers-2f-Simulation

