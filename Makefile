#
# Licence: GPLv2+
# Copyright (c) 2015-2020, Jérôme Pouiller
#
CC = gcc
CFLAGS= -Wall
PREFIX = /usr/local

all: ioctl

ioctl: ioctl.o
ifdef IGNORE_IOCTLS_LIST
ioctl: ioctls_list_empty.o
else
ioctl: ioctls_list.o
endif

ioctls_list.c: gen_ioctls_list.sh
	bash $< $(CC) > $@

%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

clean:
	rm -f *.o *.d

distclean: clean
	rm ioctl

install:
	install -s -D -m 755 ioctl $(PREFIX)/bin

-include $(wildcard *.d)

