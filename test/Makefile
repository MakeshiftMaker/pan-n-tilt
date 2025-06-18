# Based on a copy from Stefan Huber :-)
#
ROOT := "$(realpath .)"

AVR_SRCS := $(filter-out local/% rpi/%, $(wildcard */*.c *.c))
AVR_HDRS := $(filter-out local/%, $(wildcard */*.h *.h))
AVR_OBJS := $(AVR_SRCS:.c=.o)
AVR_PROGS := main fan-control \
        hello-led-bar tests/io tests/time \
        tests/ring-buffer-interrupts tests/pulse-counter \
        tests/ring-buffer-speed tests/usart-loopback \
        tests/time tests/ring-buffer-reduced \
        hello-usart hello-timer register-access-demo \
        tests/spi-echo hello-spi-slave hello-ext-interrupt play_melody play_melody_isr
AVR_PROG_OBJS := $(addsuffix .o, $(AVR_PROGS))
AVR_NO_PROG_OBJS := $(filter-out $(AVR_PROG_OBJS), $(AVR_OBJS))
AVR_HELLO_SERIAL_NONCONFLICT_ISR_DEF_OBJS := $(filter-out avrhal/usart.o avrhal/usart-string.o, $(AVR_NO_PROG_OBJS))
AVR_HELLO_TIME_NONCONFLICT_ISR_DEF_OBJS := $(filter-out avrhal/time.o avrhal/pulse-counter.o fan.o, $(AVR_NO_PROG_OBJS))

# CC and CFLAGS as passed by environment
CC_local := $(CC)
#CFLAGS_local := $(CFLAGS) -Wall -Wextra -Werror -std=c99
CFLAGS_local := $(CFLAGS) -Wall -Wextra -std=c99
# Additional CFLAGS for the tests
CFLAGS_tests = -Wno-bool-compare

# Set avr-gcc as the compiler
MCU=atmega32
CC = avr-gcc
CFLAGS = $(CFLAGS_local) -mmcu=$(MCU) -DF_CPU=8000000 -Os \
		 -g --short-enums -I$(ROOT)


%.hex: %.avrelf %.sizeinfo
	avr-objcopy -O ihex -R .eeprom $< $@

hello-usart.avrelf: $(AVR_HELLO_SERIAL_NONCONFLICT_ISR_DEF_OBJS) hello-usart.o
	$(CC) $(CFLAGS) $^ -o $@

hello-timer.avrelf: $(AVR_HELLO_TIME_NONCONFLICT_ISR_DEF_OBJS) hello-timer.o
	$(CC) $(CFLAGS) $^ -o $@

%.avrelf: $(AVR_NO_PROG_OBJS) %.o
	$(CC) $(CFLAGS) $^ -o $@

%.sizeinfo: %.avrelf
	@echo "Program -> Flash,  Data -> Internal static SRAM Usage\n\n"
	avr-size  --mcu=$(MCU) -C  $*.avrelf
clean:
	rm -rf $(AVR_OBJS)

%.local: %.c $(AVR_HDRS)
	#FIXME: Fine for now, but only works for single-file compiles
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -o $*.local $*.c

tests/local/usart-string.local: tests/local/usart-string.c avrhal/usart-string.c $(AVR_HDRS)
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -c avrhal/usart-string.c -o tests/local/usart-string.local.o
	$(CC_local) $(CFLAGS_local) $(CFLAGS_tests) -I$(ROOT) -o $@ $< tests/local/usart-string.local.o


hello-led-bar.rpi: hello-led-bar.c led-bar.c
	$(CC_local) $(CFLAGS_local) -c -o led-bar.o.rpi led-bar.c
	$(CC_local) $(CFLAGS_local) -o hello-led-bar.rpi hello-led-bar.c led-bar.o.rpi -lwiringPi

rpi/hello-spi-master.rpi: rpi/hello-spi-master.c
	$(CC_local) $(CFLAGS_local) -o rpi/hello-spi-master.rpi rpi/hello-spi-master.c -lwiringPi

rpi/fan-control-cli.rpi: rpi/fan-control-cli.c
	$(CC_local) $(CFLAGS_local) -o rpi/fan-control-cli.rpi rpi/fan-control-cli.c -I . -lwiringPi

%.upload: %.hex
	avrdude -c jtag3 -p m32 -P usb -U flash:w:$*.hex

.PHONY: clean upload
