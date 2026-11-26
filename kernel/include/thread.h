#ifndef __MERCURY__THREAD_H
#define __MERCURY__THREAD_H

int kthread_create(const char* name, void (*entry)(void));

#endif
