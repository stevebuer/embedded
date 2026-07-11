CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m0 -mthumb -Os -nostdlib -nostartfiles -Tstm32f030f4p6.ld
OBJS=main.o startup_stm32f030f4p6.o

all: blinky.bin

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

startup_stm32f030f4p6.o: startup_stm32f030f4p6.s
	$(CC) $(CFLAGS) -c startup_stm32f030f4p6.s -o startup_stm32f030f4p6.o

blinky.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o blinky.elf

blinky.bin: blinky.elf
	arm-none-eabi-objcopy -O binary blinky.elf blinky.bin

install: blinky.bin
	st-flash write blinky.bin 0x8000000

clean:
	rm -f *.o *.elf *.bin
