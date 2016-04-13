################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Components/s5k5cag/s5k5cag.c 

OBJS += \
./Utilities/Components/s5k5cag/s5k5cag.o 

C_DEPS += \
./Utilities/Components/s5k5cag/s5k5cag.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Components/s5k5cag/%.o: ../Utilities/Components/s5k5cag/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DSTM32F746G_DISCO -DSTM32F746NGHx -DSTM32F7 -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/inc" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/CMSIS/core" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/CMSIS/device" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/HAL_Driver/Inc/Legacy" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/HAL_Driver/Inc" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Middlewares/ST/STemWin/Config" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Middlewares/ST/STemWin/inc" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/ampire480272" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/ampire640480" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/Common" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/exc7200" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/ft5336" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/mfxstm32l152" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/n25q128a" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/n25q512a" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/ov9655" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/rk043fn48h" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/s5k5cag" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/st7735" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/stmpe811" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/ts3510" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Components/wm8994" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Fonts" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/Log" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities" -I"C:/Users/smith/OneDrive/STM32_Eclipse_Workspace/Senior_Design_Final_Iteration/Utilities/STM32746G-Discovery" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


