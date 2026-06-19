CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m3 -mthumb -Os -g -nostdlib -nostartfiles -Tstm32f100rb.ld
OBJS=main.o startup_stm32f100rb.o

all: blinky.bin

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

startup_stm32f100rb.o: startup_stm32f100rb.s
	$(CC) $(CFLAGS) -c startup_stm32f100rb.s -o startup_stm32f100rb.o

blinky.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o blinky.elf

blinky.bin: blinky.elf
	arm-none-eabi-objcopy -O binary blinky.elf blinky.bin

install: blinky.bin
	st-flash write blinky.bin 0x8000000

openocd:
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg

gdb: blinky.elf
	gdb-multiarch -ex "target remote localhost:3333" blinky.elf

clean:
	rm -f *.o *.elf *.bin
