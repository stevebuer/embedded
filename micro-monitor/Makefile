PROJECT = micro-monitor
MACHINE ?= linux-host

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build/$(MACHINE)
VIEWER_BIN = $(BUILD_DIR)/viewer

ifeq ($(MACHINE),linux-host)
	CC = gcc
	LD = gcc
	HAL_DIR = hal/linux-host
	CFLAGS = -std=c11 -Wall -Wextra -I$(INC_DIR) -I$(HAL_DIR)
	VIEWER_CFLAGS = -std=c11 -Wall -Wextra -I$(INC_DIR) -I$(HAL_DIR)
	VIEWER_LDFLAGS = -lncurses
	LDFLAGS =
	TARGET = $(BUILD_DIR)/$(PROJECT)
	OBJ_EXT = .o
else ifeq ($(MACHINE),mcs51)
	CC = sdcc
	AS = sdas8051
	LD = sdcc
	HAL_DIR = hal/8051
	CFLAGS = --std-sdcc99 -I$(INC_DIR) -I$(HAL_DIR) -DMCS51
	LDFLAGS =
	TARGET = $(BUILD_DIR)/micro-mon.ihx
	OBJ_EXT = .rel
else ifeq ($(MACHINE),stm8s)
	MACHINE = stm8
	CC = sdcc
	LD = sdcc
	HAL_DIR = hal/stm8s
	CFLAGS = --std-sdcc99 -mstm8 -I$(INC_DIR) -I$(HAL_DIR) -DSTM8
	LDFLAGS = -mstm8
	TARGET = $(BUILD_DIR)/micro-mon.ihx
	OBJ_EXT = .rel
else ifeq ($(MACHINE),stm8l)
	MACHINE = stm8
	CC = sdcc
	LD = sdcc
	HAL_DIR = hal/stm8l
	CFLAGS = --std-sdcc99 -mstm8 -I$(INC_DIR) -I$(HAL_DIR) -DSTM8
	LDFLAGS = -mstm8
	TARGET = $(BUILD_DIR)/micro-mon.ihx
	OBJ_EXT = .rel
else
	$(error Unsupported MACHINE='$(MACHINE)'. Use 'linux-host', 'mcs51', or 'stm8')
endif

SRC = $(wildcard $(SRC_DIR)/*.c)

ifeq ($(MACHINE),linux-host)
	HAL_SRC = $(filter-out $(HAL_DIR)/viewer.c $(HAL_DIR)/shmem-interp.c,$(wildcard $(HAL_DIR)/*.c))
else
	HAL_SRC = $(wildcard $(HAL_DIR)/*.c)
endif

OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
HAL_OBJ = $(patsubst $(HAL_DIR)/%.c,$(BUILD_DIR)/%.o,$(HAL_SRC))

ifneq ($(filter $(MACHINE),mcs51 stm8),)
	OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(SRC))
	HAL_OBJ = $(patsubst $(HAL_DIR)/%.c,$(BUILD_DIR)/%.rel,$(HAL_SRC))
endif

all: $(TARGET) $(VIEWER_BIN)

host:
	$(MAKE) MACHINE=linux-host all

viewer:
	$(MAKE) MACHINE=linux-host $(VIEWER_BIN)

mcs51:
	$(MAKE) MACHINE=mcs51 all

stm8:
	$(MAKE) MACHINE=stm8 all

stm8s:
	$(MAKE) MACHINE=stm8s all

stm8l:
	$(MAKE) MACHINE=stm8l all

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(HAL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.rel: $(HAL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ) $(HAL_OBJ)
	$(LD) $(CFLAGS) $(OBJ) $(HAL_OBJ) $(LDFLAGS) -o $@

$(VIEWER_BIN): $(BUILD_DIR)/viewer.o $(BUILD_DIR)/program.o
	$(CC) $(VIEWER_CFLAGS) -o $@ $(BUILD_DIR)/viewer.o $(BUILD_DIR)/program.o $(VIEWER_LDFLAGS)

$(BUILD_DIR)/viewer.o: $(HAL_DIR)/viewer.c | $(BUILD_DIR)
	$(CC) $(VIEWER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/program.o: $(HAL_DIR)/program.c | $(BUILD_DIR)
	$(CC) $(VIEWER_CFLAGS) -c $< -o $@

clean:
	rm -rf build *.ihx *.lk *.map *.mem *.rst *.sym

sim:
	s51 -S port=8051 t 8052 build/firmware.ihx

.PHONY: all clean sim host viewer mcs51 stm8 stm8s stm8l

