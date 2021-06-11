PREFIX = /usr/local
CC = cc
CFLAGS = -Wall -Wextra -Wno-missing-braces -pedantic -Os

covert2: main.c config.h
	gcc -o $@ $^ ${CFLAGS}

config.h: 
	cp config.def.h $@

clean:
	rm covert2

install: covert covert2
	cp -f covert ${PREFIX}/bin
	cp -f covert2 ${PREFIX}/bin
	setcap CAP_SETGID+ep /usr/local/bin/covert2

uninstall:
	rm ${PREFIX}/bin/covert
	rm ${PREFIX}/bin/covert2

.PHONY: install clean
