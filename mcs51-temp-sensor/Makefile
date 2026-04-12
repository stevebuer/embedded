PROJECT = firmware

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build

CC = sdcc
AS = sdas8051
LD = sdcc

CFLAGS = -mmcs51 --std-sdcc99 -I$(INC_DIR)

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(SRC))

all: $(BUILD_DIR) $(BUILD_DIR)/$(PROJECT).ihx

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(PROJECT).ihx: $(OBJ)
	$(LD) $(CFLAGS) $(OBJ) -o $(BUILD_DIR)/$(PROJECT).ihx

clean:
	rm -rf $(BUILD_DIR) *.ihx *.lk *.map *.mem *.rst *.sym

.PHONY: all clean
