#
# Licence: GPL
# Created: 2015-05-15 10:04:49+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CC = gcc
CFLAGS= -Wall
PREFIX = /usr/local

all: ioctl

ioctl: ioctl.o
-include ioctl.d

ioctls_list.c: genIoctlsList.sh
	./genIoctlsList.sh $(CC) > ioctls_list.c

%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

clean:
	rm -f *.o *.d

distclean: clean
	rm ioctl

install:
	install -s -D -m 755 ioctl $(PREFIX)/bin


