################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include DAP/DAP_Core/subdir.mk
-include DAP/CherryUSB/port/ch32/subdir.mk
-include DAP/CherryUSB/core/subdir.mk
-include DAP/CherryUSB/class/hid/subdir.mk
-include DAP/CherryUSB/class/cdc/subdir.mk
-include DAP/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
DAP_X033.hex \

SECONDARY_LIST += \
DAP_X033.lst \

SECONDARY_SIZE += \
DAP_X033.siz \


# All Target
all: DAP_X033.elf secondary-outputs

# Tool invocations
DAP_X033.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-elf-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -flto -Wunused -Wuninitialized  -g -T "C:\Users\ASUS\Desktop\ylj\_git\DAPLink_X033\Code\DAP_X033\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"DAP_X033.map" --specs=nano.specs --specs=nosys.specs -o "DAP_X033.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
DAP_X033.hex: DAP_X033.elf
	@	riscv-none-elf-objcopy -O ihex "DAP_X033.elf"  "DAP_X033.hex"
	@	@
DAP_X033.lst: DAP_X033.elf
	@	riscv-none-elf-objdump --all-headers --demangle --disassemble -M xw "DAP_X033.elf" > "DAP_X033.lst"
	@	@
DAP_X033.siz: DAP_X033.elf
	@	riscv-none-elf-size --format=berkeley "DAP_X033.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) DAP_X033.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
