################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Debug/debug.c 

OBJS += \
./Debug/debug.o 

C_DEPS += \
./Debug/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Debug/%.o: ../Debug/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\Debug" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\Core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\User" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\Peripheral\inc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB\class\cdc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB\class\hid" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB\common" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB\core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB\port" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP\CherryUSB" -I"C:\Users\ASUS\Desktop\ylj\_git\DAP_X033\Code\DAP_X033\DAP" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

