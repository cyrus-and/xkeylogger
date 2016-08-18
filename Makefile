.PHONY: clean

CFLAGS=-Wall -pedantic -O3 -Os
LDLIBS=-lX11 -lXi

xkeylogger: xkeylogger.c

clean:
	$(RM) xkeylogger
