CFLAGS=-Wall -pedantic -O3 -Os -fno-strict-aliasing -std=gnu99
LIBS=-lX11 -lXi

xkeylogger: xkeylogger.c
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@
