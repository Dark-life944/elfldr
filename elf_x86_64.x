/* Copyright (C) 2023 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

PHDRS {
	ph_text   PT_LOAD FLAGS (0x1 | 0x4);
	ph_rodata PT_LOAD FLAGS (0x4);
	ph_data   PT_LOAD FLAGS (0x2 | 0x4);
}

SECTIONS {
	.text : { *(.text .text.*) } : ph_text
	. = ALIGN(0x4000);
	.rodata : { *(.rodata .rodata.*) } : ph_rodata
	. = ALIGN(0x4000);
	.data : { *(.data .data.*) } : ph_data
	.bss : { *(.bss .bss.*) *(COMMON) }
}
