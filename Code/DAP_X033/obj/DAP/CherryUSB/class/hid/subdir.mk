################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DAP/CherryUSB/class/hid/usbd_hid.c 

OBJS += \
./DAP/CherryUSB/class/hid/usbd_hid.o 

C_DEPS += \
./DAP/CherryUSB/class/hid/usbd_hid.d 


# Each subdirectory must supply rules for building sources it contributes
DAP/CherryUSB/class/hid/%.o: ../DAP/CherryUSB/class/hid/%.c
	@	@	riscv-none-elf-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized  -g -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\User" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Peripheral\inc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\cdc" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\class\hid" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\common" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\core" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB\port" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\CherryUSB" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP" -I"C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\DAP\DAP_Core" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

