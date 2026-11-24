GCCPARAMS = -m32 -fPIC -shared -Wl,-soname,libc.so -Wl,--hash-style=sysv -nostdlib
C_SOURCES := $(wildcard $(addsuffix /*.c, ./))

libc.so:
	gcc -m32 -fPIC -shared -Wl,-soname,libc.so -Wl,--hash-style=sysv -nostdlib -o libc.so $(C_SOURCES)
