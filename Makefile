PREFIX = /usr/local
CC = cc
CFLAGS = -Wall -Wextra -Wno-missing-braces -pedantic -Os

covert2: main.c
	gcc -o $@ $^ ${CFLAGS}

clean:
	rm covert2

install: covert covert2
	cp -f covert ${PREFIX}/bin
	cp -f covert2 ${PREFIX}/bin
# TODO?
	cp config.def /etc/covert
	setcap CAP_SETGID+ep /usr/local/bin/covert2

uninstall:
	@echo note: /etc/covert doesn\'t get removed automatically to prevent data loss.
	@echo you have to remove it by hand
	rm ${PREFIX}/bin/covert
	rm ${PREFIX}/bin/covert2

.PHONY: install clean
