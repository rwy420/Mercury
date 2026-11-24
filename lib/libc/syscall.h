#ifndef __SYSCALL_H
#define __SYSCALL_H

static inline int syscall(int syscall, int ebx, int ecx, int edx, int esi, int edi)
{
    int ret;
    asm volatile (
        "int $0x80"
        : "=a" (ret)
        : "a" (syscall),
          "b" (ebx),
          "c" (ecx),
          "d" (edx),
          "S" (esi),
          "D" (edi)
    );

    return ret;
}

#endif
