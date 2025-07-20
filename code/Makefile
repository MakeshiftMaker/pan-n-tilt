# Root path of the project
ROOT := $(realpath .)

# Source and header files
AVR_SRCS := $(filter-out local/% rpi/%, $(wildcard */*.c *.c))
AVR_HDRS := $(filter-out local/%, $(wildcard */*.h *.h))
AVR_OBJS := $(AVR_SRCS:.c=.o)

# List of programs to build (adjust to your .c file names)
AVR_PROGS := main fan-control \
        hello-led-bar tests/io tests/time \
        tests/ring-buffer-interrupts tests/pulse-counter \
        tests/ring-buffer-speed tests/usart-loopback \
        tests/ring-buffer-reduced \
        hello-usart hello-timer register-access-demo \
        tests/spi-echo hello-spi-slave hello-ext-interrupt \
        play_melody play_melody_isr

AVR_PROG_OBJS := $(addsuffix .o, $(AVR_PROGS))
AVR_NO_PROG_OBJS := $(filter-out $(AVR_PROG_OBJS), $(AVR_OBJS))

AVR_HELLO_SERIAL_NONCONFLICT_ISR_DEF_OBJS := $(filter-out avrhal/usart.o avrhal/usart-string.o, $(AVR_NO_PROG_OBJS))
AVR_HELLO_TIME_NONCONFLICT_ISR_DEF_OBJS := $(filter-out avrhal/time.o avrhal/pulse-counter.o fan.o, $(AVR_NO_PROG_OBJS))

# Compiler and flags
MCU ?= atmega328p
F_CPU ?= 16000000UL
CC = avr-gcc
CFLAGS = -Wall -Wextra -std=c99 -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os \
         -g --short-enums -I$(ROOT)

# Local (host-side) compiler settings for simulation/tests
CC_local := gcc
CFLAGS_local := -Wall -Wextra -std=c99
CFLAGS_tests = -Wno-bool-compare

# Flashing tool (bootloader upload over USB)
AVRDUDE_PROGRAMMER ?= arduino
AVRDUDE_PORT ?= /dev/ttyUSB0
AVRDUDE_BAUD ?= 57600 #115200

# Build ELF and HEX
%.avrelf: $(AVR_NO_PROG_OBJS) %.o
	$(CC) $(CFLAGS) $^ -o $@

hello-usart.avrelf: $(AVR_HELLO_SERIAL_NONCONFLICT_ISR_DEF_OBJS) hello-usart.o
	$(CC) $(CFLAGS) $^ -o $@

hello-timer.avrelf: $(AVR_HELLO_TIME_NONCONFLICT_ISR_DEF_OBJS) hello-timer.o
	$(CC) $(CFLAGS) $^ -o $@

%.hex: %.avrelf %.sizeinfo
	avr-objcopy -O ihex -R .eeprom $< $@

%.sizeinfo: %.avrelf
	@echo "Program -> Flash,  Data -> Internal static SRAM Usage\n\n"
	avr-size --mcu=$(MCU) -C $<

# Flash the hex file to the Arduino
%.upload: %.hex
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -P $(AVRDUDE_PORT) -b $(AVRDUDE_BAUD) -U flash:w:$*.hex

# Build for local simulation/testing
%.local: %.c $(AVR_HDRS)
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -o $*.local $*.c

tests/local/usart-string.local: tests/local/usart-string.c avrhal/usart-string.c $(AVR_HDRS)
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -c avrhal/usart-string.c -o tests/local/usart-string.local.o
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -o $@ $< tests/local/usart-string.local.o

# For building on Raspberry Pi with wiringPi (can be removed if not used)
hello-led-bar.rpi: hello-led-bar.c led-bar.c
	$(CC_local) $(CFLAGS_local) -c -o led-bar.o.rpi led-bar.c
	$(CC_local) $(CFLAGS_local) -o hello-led-bar.rpi hello-led-bar.c led-bar.o.rpi -lwiringPi

rpi/hello-spi-master.rpi: rpi/hello-spi-master.c
	$(CC_local) $(CFLAGS_local) -o rpi/hello-spi-master.rpi rpi/hello-spi-master.c -lwiringPi

rpi/fan-control-cli.rpi: rpi/fan-control-cli.c
	$(CC_local) $(CFLAGS_local) -o rpi/fan-control-cli.rpi rpi/fan-control-cli.c -I . -lwiringPi

# Clean build artifacts
clean:
	rm -rf $(AVR_OBJS) *.elf *.hex *.avrelf *.o *.local *.sizeinfo *.rpi
