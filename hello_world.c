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

#include "payload.h"


static __attribute__ ((used)) long ptr_syscall;


asm(".intel_syntax noprefix\n"
    ".global syscall\n"
    ".type syscall @function\n"
    "syscall:\n"
    "  mov rax, rdi\n"                      // sysno
    "  mov rdi, rsi\n"                      // arg1
    "  mov rsi, rdx\n"                      // arg2
    "  mov rdx, rcx\n"                      // arg3
    "  mov r10, r8\n"                       // arg4
    "  mov r8,  r9\n"                       // arg5
    "  mov r9,  qword ptr [rsp + 8]\n"      // arg6
    "  jmp qword ptr [rip + ptr_syscall]\n" // syscall
    "  ret\n"
    );

long syscall(long sysno, ...);


void
klog(const char *s) {
  syscall(0x259, 7, "<118>", 0);
  syscall(0x259, 7, s, 0);
  syscall(0x259, 7, "\n", 0);
}


void
_start(const payload_args_t *args) {
  void (*puts)(const char*);

  if((args->sceKernelDlsym(0x2, "puts", &puts))) {
    return;
  }
  if(args->sceKernelDlsym(0x2001, "getpid", &ptr_syscall)) {
    return;
  }
  ptr_syscall += 0xa; // jump directly to syscall instruction


  klog("[hello_world.elf] Hello, world!");
  puts("[hello_world.elf] Hello, world!");
}

