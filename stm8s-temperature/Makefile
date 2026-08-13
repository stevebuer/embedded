###############################################################################
# Project configuration
###############################################################################

# Name of the final firmware file (without extension)
TARGET      := aht20-monitor

# MCU family (used for include paths and optional flags)
MCU         := STM8S003

# Path to the stm8s-sdcc SPL 
SPL_PATH    := $(HOME)/GITHUB/STM8-SPL-SDCC/Libraries/STM8S_StdPeriph_Driver

# Project source files
SRC_DIR     := src
INC_DIR	    := inc
SOURCES     := $(wildcard $(SRC_DIR)/*.c)

# Only compile SPL modules used by this application. Linking every source in a
# peripheral family pulls in many unused APIs because each API has its own file.
SPL_SOURCE_NAMES := \
	stm8s_clk_DeInit.c \
	stm8s_clk_GetClockFreq.c \
	stm8s_clk_HSIPrescalerConfig.c \
	stm8s_clk_PeripheralClockConfig.c \
	stm8s_clk_private.c \
	stm8s_gpio_Init.c \
	stm8s_uart1_DeInit.c \
	stm8s_uart1_Init.c \
	stm8s_uart1_GetFlagStatus.c \
	stm8s_uart1_SendData8.c \
	stm8s_uart1_ReceiveData8.c \
	stm8s_i2c_AcknowledgeConfig.c \
	stm8s_i2c_CheckEvent.c \
	stm8s_i2c_ClearFlag.c \
	stm8s_i2c_Cmd.c \
	stm8s_i2c_DeInit.c \
	stm8s_i2c_GenerateSTART.c \
	stm8s_i2c_GenerateSTOP.c \
	stm8s_i2c_GetFlagStatus.c \
	stm8s_i2c_Init.c \
	stm8s_i2c_ReceiveData.c \
	stm8s_i2c_Send7bitAddress.c \
	stm8s_i2c_SendData.c \
	stm8s_i2c_SoftwareResetCmd.c \
	stm8s_flash_Unlock.c \
	stm8s_flash_ProgramByte.c \
	stm8s_flash_Lock.c \
	stm8s_flash_ReadByte.c \
	stm8s_flash_private.c
SPL_SOURCES := $(addprefix $(SPL_PATH)/src/,$(SPL_SOURCE_NAMES))

# Output directory
BUILD_DIR   := build

###############################################################################
# Compiler / Linker settings
###############################################################################

CC          := sdcc
OBJCOPY     := packihx
FLASH       := stm8flash
PROGRAMMER  ?= stlinkv2

CFLAGS      := -mstm8 -D$(MCU) \
               --std-sdcc99 \
               --opt-code-size \
               -I$(SRC_DIR) \
	       -I$(INC_DIR) \
               -I$(SPL_PATH)/inc \

LDFLAGS     := -mstm8

###############################################################################
# Build rules
###############################################################################

# Object files
APP_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(SOURCES))
SPL_OBJECTS := $(patsubst $(SPL_PATH)/src/%.c,$(BUILD_DIR)/spl_%.rel,$(SPL_SOURCES))
OBJECTS     := $(APP_OBJECTS) $(SPL_OBJECTS)

# Default target
all: $(BUILD_DIR)/$(TARGET).ihx $(BUILD_DIR)/$(TARGET).bin

check-spl:
	@if ! ls "$(SPL_PATH)/inc" "$(SPL_PATH)/src" >/dev/null 2>&1; then \
		echo "error: SPL path is not valid: $(SPL_PATH)"; \
		echo "hint: set SPL_PATH to STM8S_StdPeriph_Driver root"; \
		exit 1; \
	fi

# Create build directory if missing
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C → .rel
$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/spl_%.rel: $(SPL_PATH)/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link → .ihx
$(BUILD_DIR)/$(TARGET).ihx: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Convert .ihx → .bin
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).ihx
	$(OBJCOPY) $< > $(BUILD_DIR)/$(TARGET).hex
	hex2bin $(BUILD_DIR)/$(TARGET).hex

# Flash device
flash: $(BUILD_DIR)/$(TARGET).ihx
	$(FLASH) -c $(PROGRAMMER) -p stm8s003f3 -s flash -w $<

###############################################################################
# Cleaning
###############################################################################

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean check-spl flash
