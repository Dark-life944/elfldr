/* Copyright (C) 2024 John Törnblom

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

#include <ps5/kernel.h>
#include <ps5/klog.h>

#include "elfldr.h"
#include "pt.h"

#include "bootstrap_elf.c"


/**
 * sceKernelSpawn() is not available in libkernel_web, which is what is used by
 * the webkit exploit entry point. However, we do not actually use it initially,
 * hence we just define an empty stub to silence the linker.
 **/
int
sceKernelSpawn(int *pid, int dbg, const char *path, char *root,
	       char* argv[]) {
  return -1;
}


/**
 * We are running inside bdj.elf, attach to SceRedisServer and run bootstrap.elf.
 **/
int
main() {
  pid_t mypid = getpid();
  uint8_t qa_flags[16];
  uint8_t caps[16];
  uint64_t authid;
  intptr_t vnode;
  pid_t vpid;
  int ret;

  klog_puts("Bootstrapping elfldr.elf...");

  // enable debugging with ptrace
  if(kernel_get_qaflags(qa_flags)) {
    klog_puts("kernel_get_qa_flags failed");
    return -1;
  }
  qa_flags[1] |= 0x03;
  if(kernel_set_qaflags(qa_flags)) {
    klog_puts("kernel_set_qa_flags failed");
    return -1;
  }

  // backup my privileges
  if(!(vnode=kernel_get_proc_rootdir(mypid))) {
    klog_puts("kernel_get_proc_rootdir failed");
    return -1;
  }
  if(kernel_get_ucred_caps(mypid, caps)) {
    klog_puts("kernel_get_ucred_caps failed");
    return -1;
  }
  if(!(authid=kernel_get_ucred_authid(mypid))) {
    klog_puts("kernel_get_ucred_authid failed");
    return -1;
  }

  // launch bootstrap.elf inside SceRedisServer
  if((vpid=elfldr_find_pid("SceRedisServer")) < 0) {
    klog_puts("elfldr_find_pid failed");
    return -1;
  } else if(elfldr_raise_privileges(mypid)) {
    klog_puts("Unable to raise privileges");
    ret = -1;
  } else if(pt_attach(vpid)) {
    klog_perror("pt_attach");
    ret = -1;
  } else {
    ret = elfldr_exec(vpid, -1, bootstrap_elf);
  }

  // restore my privileges
  if(kernel_set_proc_jaildir(mypid, vnode)) {
    klog_puts("kernel_set_proc_jaildir failed");
    ret = -1;
  }
  if(kernel_set_proc_rootdir(mypid, vnode)) {
    klog_puts("kernel_set_proc_rootdir failed");
    ret = -1;
  }
  if(kernel_set_ucred_caps(mypid, caps)) {
    klog_puts("kernel_set_ucred_caps failed");
    ret = -1;
  }
  if(kernel_set_ucred_authid(mypid, authid)) {
    klog_puts("kernel_set_ucred_authid failed");
    ret = -1;
  }

  return ret;
}

