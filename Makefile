.PHONY: dynamic static clean

CFLAGS = -Wall -pedantic -O3 -Os
LDFLAGS = -s

dynamic: LDLIBS += -lX11 -lXi
dynamic: xkeylogger

static: LDLIBS += -Wl,-Bstatic -lX11 -lXi -lxcb -lXau -lXdmcp -lXext -Wl,-Bdynamic
static: xkeylogger

clean:
	$(RM) xkeylogger
