CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

SRC = ordonnanceur.c
TARGET = ordonnanceur

MCU = atmega328p
F_CPU = 16000000
PORT = /dev/ttyACM0
BAUD = 115200

# Règles
all: $(TARGET).hex

%.elf: %.c ordonnanceur.h
	$(CC) -mmcu=$(MCU) -Wall -Werror -I. -DF_CPU=$(F_CPU) -Os -o $@ $<

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

flash: $(TARGET).hex
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -b $(BAUD) -D -U flash:w:$<

clean:
	rm -f *.elf *.hex

