################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Derivative_Filter.cpp \
../src/Multi_Page.cpp \
../src/System_Variable_Settings.cpp \
../src/bsp.cpp \
../src/encoder.cpp \
../src/main.cpp \
../src/motor.cpp \
../src/pid_controller.cpp \
../src/stm32f7xx_hal_msp.cpp \
../src/stm32f7xx_it.cpp \
../src/system_stm32f7xx.cpp 

OBJS += \
./src/Derivative_Filter.o \
./src/Multi_Page.o \
./src/System_Variable_Settings.o \
./src/bsp.o \
./src/encoder.o \
./src/main.o \
./src/motor.o \
./src/pid_controller.o \
./src/stm32f7xx_hal_msp.o \
./src/stm32f7xx_it.o \
./src/system_stm32f7xx.o 

CPP_DEPS += \
./src/Derivative_Filter.d \
./src/Multi_Page.d \
./src/System_Variable_Settings.d \
./src/bsp.d \
./src/encoder.d \
./src/main.d \
./src/motor.d \
./src/pid_controller.d \
./src/stm32f7xx_hal_msp.d \
./src/stm32f7xx_it.d \
./src/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo %cd%
	arm-none-eabi-g++ -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DSTM32F746G_DISCO -DSTM32F746NGHx -DSTM32F7 -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/inc" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/CMSIS/core" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/CMSIS/device" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/HAL_Driver/Inc/Legacy" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/HAL_Driver/Inc" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Middlewares/ST/STemWin/Config" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Middlewares/ST/STemWin/inc" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/ampire480272" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/ampire640480" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/Common" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/exc7200" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/ft5336" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/mfxstm32l152" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/n25q128a" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/n25q512a" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/ov9655" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/rk043fn48h" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/s5k5cag" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/st7735" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/stmpe811" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/ts3510" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Components/wm8994" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Fonts" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/Log" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities" -I"C:/Users/smith/Documents/GitHub/NRC_Senior_Design/Senior_Design_Eclipse_Workspace/Utilities/STM32746G-Discovery" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


