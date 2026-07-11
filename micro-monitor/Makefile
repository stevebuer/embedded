PROJECT = micro-mon

MACHINE=mcs51

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build
HAL_DIR = hal/8051

CC = sdcc
AS = sdas8051
LD = sdcc

CFLAGS = --std-sdcc99 -I$(INC_DIR) -I$(HAL_DIR) -DMCS51

SRC = $(wildcard $(SRC_DIR)/*.c)
HAL_SRC = $(HAL_DIR)/serial.c

OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(SRC))
HAL_OBJ = $(patsubst $(HAL_DIR)/%.c,$(BUILD_DIR)/%.rel,$(HAL_SRC))

all: $(BUILD_DIR) $(BUILD_DIR)/$(PROJECT).ihx

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.rel: $(HAL_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(PROJECT).ihx: $(OBJ) $(HAL_OBJ)
	$(LD) $(CFLAGS) $(OBJ) $(HAL_OBJ) -o $(BUILD_DIR)/$(PROJECT).ihx

clean:
	rm -rf $(BUILD_DIR) *.ihx *.lk *.map *.mem *.rst *.sym

sim:
	s51 -S port=8051 t 8052 build/firmware.ihx

.PHONY: all clean sim

