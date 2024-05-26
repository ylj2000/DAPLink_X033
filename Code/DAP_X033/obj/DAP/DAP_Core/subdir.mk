################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DAP/DAP_Core/DAP.c \
../DAP/DAP_Core/JTAG_DP.c \
../DAP/DAP_Core/SW_DP.c 

OBJS += \
./DAP/DAP_Core/DAP.o \
./DAP/DAP_Core/JTAG_DP.o \
./DAP/DAP_Core/SW_DP.o 

C_DEPS += \
./DAP/DAP_Core/DAP.d \
./DAP/DAP_Core/JTAG_DP.d \
./DAP/DAP_Core/SW_DP.d 


# Each subdirectory must supply rules for building sources it contributes
DAP/DAP_Core/%.o: ../DAP/DAP_Core/%.c
	@	@	riscv-none-elf-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized  -g -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\User" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Peripheral\inc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\cdc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\hid" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\common" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\port" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\DAP_Core" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

