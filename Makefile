default: textclock

all: textclock debug

textclock: textclock.c util.h util.c xcbft.h utf8.h 
	cc -O3 -Wall -o textclock textclock.c util.c -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -I/usr/include/xcb -pthread -lfontconfig -lfreetype -lxcb -lxcb-ewmh -lxcb-render -lxcb-render-util -lxcb-xrm

debug: textclock.c util.h util.c xcbft.h utf8.h
	cc -g -Wall -o textclock-dbg textclock.c util.c -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -I/usr/include/xcb -pthread -lfontconfig -lfreetype -lxcb -lxcb-ewmh -lxcb-render -lxcb-render-util -lxcb-xrm

run: textclock
	./textclock

install: textclock
	install -D -m0755  textclock $(HOME)/.local/bin
	sed "s|@@|$(HOME)|" textclock.desktop.in > textclock.desktop
	install -D -m0644  textclock.desktop $(HOME)/.local/share/applications
