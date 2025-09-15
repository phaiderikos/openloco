# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = software


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
# QSpy enabled
Q_SPY = 0

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build
# CubeMX Repository path
REPO_DIR ?= ../../../../STM32Cube/Repository
#pass as an argument to the make command

######################################
# source
######################################
# C sources
C_SOURCES =  \
core/src/main.c \
core/src/bsp/bsp.c \
core/src/bsp/bsp_gpio.c \
core/src/bsp/bsp_timer.c \
core/src/bsp/bsp_uart.c \
core/src/stm32l0xx_it.c \
core/src/stm32l0xx_hal_msp.c \
core/src/system_stm32l0xx.c \
core/src/syscalls.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_exti.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c \
$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c \
qpc/src/qv/qv.c \
qpc/src/qf/qep_hsm.c \
qpc/src/qf/qep_msm.c \
qpc/src/qf/qf_act.c \
qpc/src/qf/qf_actq.c \
qpc/src/qf/qf_defer.c \
qpc/src/qf/qf_dyn.c \
qpc/src/qf/qf_mem.c \
qpc/src/qf/qf_ps.c \
qpc/src/qf/qf_qact.c \
qpc/src/qf/qf_qeq.c \
qpc/src/qf/qf_qmact.c \
qpc/src/qf/qf_time.c \
qpc/ports/arm-cm/qv/gnu/qv_port.c \
core/src/dcc/qm/qm_controller.c \
core/src/dcc/qm/qm_motor.c \
core/src/dcc/qm/qm_decoder.c \
core/src/dcc/cv.c \
core/src/dcc/dcc_funct.c \
core/src/dcc/decoder.c \
core/src/dcc/motor.c

ifeq ($(Q_SPY), 1)
C_SOURCES += qpc/src/qs/qs.c \
qpc/src/qs/qs_64bit.c \
qpc/src/qs/qs_fp.c \
qpc/src/qs/qs_rx.c \
qpc/src/qs/qstamp.c
endif

# ASM sources
ASM_SOURCES =  \
startup_stm32l031g6ux.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
OD = $(PREFIX)/$(PREFIX)objdump
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
OD = $(PREFIX)objdump
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0plus

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L031xx \
-DQP_API_VERSION=734

ifeq ($(Q_SPY), 1)
C_DEFS += -DQ_SPY
endif

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Icore/inc \
-Icore/src/bsp \
-Icore/inc/dcc \
-Icore/src/dcc/qm \
-I$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Inc \
-I$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
-I$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
-I$(REPO_DIR)/STM32Cube_FW_L0_V1.12.2/Drivers/CMSIS/Include \
-Iqpc/include \
-Iqpc/ports/arm-cm/qv/gnu \
-Iqpc/ports/config

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L031G6UX_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections,--print-memory-usage

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@ -Ax

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

######################################
# generate disassembly
######################################
info: $(BUILD_DIR)/$(TARGET).elf
	$(OD) -x -S $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/$(TARGET).lst
	$(OD) -D $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/$(TARGET).dis
	$(SZ) $(BUILD_DIR)/$(TARGET).elf > $(BUILD_DIR)/$(TARGET).size

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
