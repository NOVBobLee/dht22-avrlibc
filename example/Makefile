ARCH := avr
MCU := atmega328p
MCU_PARTNO := m328p
F_CPU := 16000000UL
BAUD := 115200
PORT := /dev/ttyACM0

CC := $(ARCH)-gcc
AR := $(ARCH)-ar
OBJCOPY := $(ARCH)-objcopy

CFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Os -mmcu=$(MCU) -Wall
CFLAGS += -ffunction-sections -fdata-sections
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
CFLAGS += --param=min-pagesize=0

INC := ..
CFLAGS += -I$(INC)

# dht22 debug raw data
#CFLAGS += -D_DHT_DEBUG

LDFLAGS = -Wl,--gc-sections
# for decimal-point format in printf()
# https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaa3b98c0d17b35642c0f3e4649092b9f1
LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

TARGET := main
HEX := $(TARGET).hex
ELF := $(TARGET).elf
OBJ := $(TARGET).o print_uart.o $(INC)/dht22.o

.PHONY: all clean upload
all: $(ELF)

$(ELF): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

upload: $(HEX)
	sudo sh -c "avrdude -F -V -v -c arduino -p $(MCU_PARTNO) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX)"

$(HEX): $(ELF)
	$(OBJCOPY) -R .eeprom -O ihex $< $@

clean:
	$(RM) *.hex *.elf *.o $(OBJ)

# print the including headers
# avr-gcc -M main.c
