################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32x035_it.c \
../User/main.c \
../User/system_ch32x035.c 

OBJS += \
./User/ch32x035_it.o \
./User/main.o \
./User/system_ch32x035.o 

C_DEPS += \
./User/ch32x035_it.d \
./User/main.d \
./User/system_ch32x035.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-elf-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized  -g -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\User" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Peripheral\inc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\cdc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\hid" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\common" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\port" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\DAP_Core" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

