AS = as31
SRC = forth.asm
HEX = forth.hex

all: $(HEX)

$(HEX): $(SRC)
	$(AS) $(SRC)

clean:
	rm -f *.hex *.lst *.obj *.sym
