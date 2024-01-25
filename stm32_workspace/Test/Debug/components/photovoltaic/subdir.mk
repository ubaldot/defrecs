################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../components/photovoltaic/pv_standard_main.c 

OBJS += \
./components/photovoltaic/pv_standard_main.o 

C_DEPS += \
./components/photovoltaic/pv_standard_main.d 


# Each subdirectory must supply rules for building sources it contributes
components/photovoltaic/%.o components/photovoltaic/%.su components/photovoltaic/%.cyclo: ../components/photovoltaic/%.c components/photovoltaic/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"/Users/ubaldot/Documents/embedded/stm32_workspace/Test/components" -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/STM32F4xx_HAL_Driver/Inc -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Drivers/CMSIS/Include -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Middlewares/Third_Party/FreeRTOS/Source/include -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I/Users/ubaldot/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-components-2f-photovoltaic

clean-components-2f-photovoltaic:
	-$(RM) ./components/photovoltaic/pv_standard_main.cyclo ./components/photovoltaic/pv_standard_main.d ./components/photovoltaic/pv_standard_main.o ./components/photovoltaic/pv_standard_main.su

.PHONY: clean-components-2f-photovoltaic

