.POSIX:

BIN = prog
CC = sdcc
CPPFLAGS =
CFLAGS = --use-non-free -mpic14 -p16f877a ${CPPFLAGS}
PIC = PIC16F877A

SRC = main.c \
      tmr0.c \
      lcd.c \
      i2c.c \
      bme280.c \
      util.c

OBJ = ${SRC:.c=.o}

all: options ${BIN}

options:
	@echo ${BIN} build options:
	@echo "CC	= ${CC}"
	@echo "CFLAGS	= ${CFLAGS}"

${BIN}: ${OBJ}
	${CC} ${OBJ} ${CFLAGS} -o $@

.c.o:
	${CC} -c ${CFLAGS} $<

flash: all
	pk2cmd -P ${PIC} -X -M -F ${BIN}.hex

erase: all
	pk2cmd -P ${PIC} -E -F ${BIN}.hex

clean:
	rm -f *.hex *.asm *.cod *.hex *.lst *.o *.core

.PHONY: all options clean erase flash
