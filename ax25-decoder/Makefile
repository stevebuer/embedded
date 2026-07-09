# STM32F030F4P6 AX.25 Decoder Makefile

ARM_GCC = arm-none-eabi-gcc
ARM_AS = arm-none-eabi-as
ARM_AR = arm-none-eabi-ar
ARM_LD = arm-none-eabi-ld
ARM_OBJCOPY = arm-none-eabi-objcopy
ARM_OBJDUMP = arm-none-eabi-objdump
ARM_SIZE = arm-none-eabi-size

# Target device
TARGET = stm32f030f4p6
DEVICE = STM32F030x6

# Compiler flags
MCPU = -mcpu=cortex-m0
MTHUMB = -mthumb
MFLOAT_ABI = -mfloat-abi=soft

CFLAGS = $(MCPU) $(MTHUMB) $(MFLOAT_ABI) -Wall -Wextra -O2 -g
CFLAGS += -D$(DEVICE) -DHSE_VALUE=8000000 -Iinc
ASFLAGS = $(MCPU) $(MTHUMB) -g
LDFLAGS = $(MCPU) $(MTHUMB) --specs=nosys.specs -nostartfiles -Wl,-Map=build/output.map -Tstm32f030f4p6.ld

# Source files
SRC_C = src/main.c src/adc.c src/goertzel.c src/decoder.c src/serial.c src/tnc2.c src/buttons.c
SRC_S = src/startup.s

# Object files
OBJ_C = $(SRC_C:src/%.c=build/%.o)
OBJ_S = $(SRC_S:src/%.s=build/%.o)
OBJ = $(OBJ_C) $(OBJ_S)

# Output files
ELF = build/$(TARGET).elf
BIN = build/$(TARGET).bin
HEX = build/$(TARGET).hex

# Directories
BUILD_DIR = build
SRC_DIR = src

.PHONY: all clean size flash

all: $(BIN) $(HEX) size

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files
build/%.o: src/%.c | $(BUILD_DIR)
	$(ARM_GCC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Assemble ASM files
build/%.o: src/%.s | $(BUILD_DIR)
	$(ARM_AS) $(ASFLAGS) $< -o $@

# Link
$(ELF): $(OBJ)
	$(ARM_GCC) $(LDFLAGS) $(OBJ) -lm -o $@

# Generate binary
$(BIN): $(ELF)
	$(ARM_OBJCOPY) -O binary $< $@

# Generate hex
$(HEX): $(ELF)
	$(ARM_OBJCOPY) -O ihex $< $@

# Display size
size: $(ELF)
	$(ARM_SIZE) $(ELF)

# Disassembly
disasm: $(ELF)
	$(ARM_OBJDUMP) -d $(ELF) | tee build/disassembly.txt

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Help
help:
	@echo "Available targets:"
	@echo "  make all      - Build firmware (default)"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make size     - Show binary size"
	@echo "  make disasm   - Generate disassembly"
	@echo "  make help     - Show this help"
