PREFIX = /usr/local
CC = cc
CFLAGS = -Wall -Wextra -Wno-missing-braces -pedantic -Os

covert: main.c helpers.c
	gcc -o $@ $^ ${CFLAGS}

clean:
	rm covert

install:
	cp -f covert ${PREFIX}/bin
	setcap CAP_SETGID+ep /usr/local/bin/covert

uninstall:
	rm ${PREFIX}/bin/covert

.PHONY: install clean
