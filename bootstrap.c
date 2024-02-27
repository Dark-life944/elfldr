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

#include <signal.h>
#include <stdio.h>

#include <ps5/kernel.h>

#include "elfldr.h"
#include "klog.h"
#include "pt.h"

#include "socksrv_elf.c"


/**
 * We are running inside SceSpZeroConf, spawn socksrv.elf.
 **/
int
main() {
  pid_t mypid = getpid();
  uint8_t caps[16];
  uint64_t authid;
  intptr_t vnode;
  int ret;

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

  // launch socksrv.elf in a new processes
  if(elfldr_raise_privileges(mypid)) {
    klog_puts("Unable to raise privileges");
    ret = -1;
  } else {
    signal(SIGCHLD, SIG_IGN);
    ret = elfldr_spawn(-1, socksrv_elf);
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
