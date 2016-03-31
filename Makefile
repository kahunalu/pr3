CC=avr-gcc
COPY=avr-objcopy
LOAD=avrdude
FLAGS=-g -Os -mmcu=atmega2560 -c
ELFFLAGS= -g -mmcu=atmega2560 -o
HEXFLAGS=-j .text -j .data -O ihex
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
LOADFLAGS= -p m2560 -c stk500v2 -P /dev/ttyACM0 -b 115200 -U flash:w:img.hex:i -V -v -D
endif
ifeq ($(UNAME), Darwin)
LOADFLAGS= -p m2560 -c stk500v2 -P /dev/cu.usbmodem1411 -b 115200 -U flash:w:img.hex:i -V -v -D
endif


all: clean compile elf hex load

compile: cswitch.S os.c roomba_driver.c
	$(CC) $(FLAGS) os.c
	$(CC) $(FLAGS) cswitch.S
	$(CC) $(FLAGS) roomba_driver.c

elf: cswitch.o os.o
	$(CC) $(ELFFLAGS) img.elf cswitch.o os.o

hex: img.elf
	$(COPY) $(HEXFLAGS) img.elf img.hex

load:
	$(LOAD) $(LOADFLAGS)

clean:
	rm *.o *.hex *.elf

usart_test: usart_test.c
	$(CC) $(FLAGS) usart_test.c
	$(CC) $(ELFFLAGS) img.elf cswitch.o os.o roomba_driver.o usart_test.o

test: compile usart_test hex load

base_station: base_station.c
	$(CC) $(FLAGS) base_station.c
	$(CC) $(ELFFLAGS) img.elf cswitch.o os.o roomba_driver.o base_station.o

base_station: compile base_station hex load

remote_station: remote_station.c
	$(CC) $(FLAGS) usart_test.c
	$(CC) $(ELFFLAGS) img.elf cswitch.o os.o roomba_driver.o remote_station.o

remote_station: compile remote_station hex load
