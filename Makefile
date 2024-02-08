default: textclock

all: textclock debug

textclock: textclock.c util.h util.c
	cc -O3 -Wall -o textclock textclock.c util.c -lxcb -lxcb-ewmh

debug: textclock.c util.h util.c
	cc -g -Wall -o textclock-dbg textclock.c util.c -lxcb -lxcb-ewmh

run: textclock
	./textclock

install: textclock
	install -D -m0755  textclock $(HOME)/.local/bin
