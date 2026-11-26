#ifndef __WAIT_H
#define __WAIT_H

#include "./types.h"

pid_t wait4(pid_t pid, int wstatus, int options, void* rusage/*TODO*/);

#endif
