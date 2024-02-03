#   Copyright (C) 2024 John Törnblom
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING. If not see
# <http://www.gnu.org/licenses/>.

ifndef PS5_PAYLOAD_SDK
    $(error PS5_PAYLOAD_SDK is undefined)
endif

PS5_HOST ?= ps5
PS5_PORT ?= 9020

CC := $(PS5_PAYLOAD_SDK)/host/x86_64-ps5-payload-cc
LD := $(PS5_PAYLOAD_SDK)/host/x86_64-ps5-payload-ld

CFLAGS := -O2

all: bootstrap.elf

elfldr.elf: main.o pt.o elfldr.o
	$(LD) $^ -lSceLibcInternal -lkernel_sys -o $@

bootstrap.elf: bootstrap.o pt.o elfldr.o
	$(LD) $^ -lSceLibcInternal -lkernel_web -o $@

bootstrap.o: bootstrap.c elfldr_elf.c
	$(CC) -c $(CFLAGS) -o $@ $<

elfldr_elf.c: elfldr.elf
	xxd -i $^ > $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -f *.o bootstrap.elf elfldr.elf elfldr_elf.c

test: bootstrap.elf
	nc -q0 $(PS5_HOST) $(PS5_PORT) < $^

