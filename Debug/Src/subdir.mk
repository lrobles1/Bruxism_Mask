################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/EMGFilters.c \
../Src/adc.c \
../Src/detection.c \
../Src/dma.c \
../Src/hm_10_uart.c \
../Src/led.c \
../Src/main.c \
../Src/motor.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/usart2.c 

OBJS += \
./Src/EMGFilters.o \
./Src/adc.o \
./Src/detection.o \
./Src/dma.o \
./Src/hm_10_uart.o \
./Src/led.o \
./Src/main.o \
./Src/motor.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/usart2.o 

C_DEPS += \
./Src/EMGFilters.d \
./Src/adc.d \
./Src/detection.d \
./Src/dma.d \
./Src/hm_10_uart.d \
./Src/led.d \
./Src/main.d \
./Src/motor.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/usart2.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L4 -DSTM32 -DSTM32L476RGTx -c -I../Inc -I"C:/Users/lrob1/STM32CubeIDE/workspace_2.0.0/ENGR498_Final_Project/CMSIS/Include" -I"C:/Users/lrob1/STM32CubeIDE/workspace_2.0.0/ENGR498_Final_Project/CMSIS/Device/ST/STM32L4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/EMGFilters.cyclo ./Src/EMGFilters.d ./Src/EMGFilters.o ./Src/EMGFilters.su ./Src/adc.cyclo ./Src/adc.d ./Src/adc.o ./Src/adc.su ./Src/detection.cyclo ./Src/detection.d ./Src/detection.o ./Src/detection.su ./Src/dma.cyclo ./Src/dma.d ./Src/dma.o ./Src/dma.su ./Src/hm_10_uart.cyclo ./Src/hm_10_uart.d ./Src/hm_10_uart.o ./Src/hm_10_uart.su ./Src/led.cyclo ./Src/led.d ./Src/led.o ./Src/led.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/motor.cyclo ./Src/motor.d ./Src/motor.o ./Src/motor.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/usart2.cyclo ./Src/usart2.d ./Src/usart2.o ./Src/usart2.su

.PHONY: clean-Src

