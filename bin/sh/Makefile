GCCPARAMS = -m32 -Iinclude -I../../lib/libc -nostdlib -nostdinc -ffreestanding -fpie
LDPARAMS = -m32 -pie -nostdlib -nostdinc -I../../lib/libc -fno-builtin -nostartfiles -ffreestanding -Wl,-y,write \
		   -Wl,-y,_start -Wl,-e_start -Wl,--hash-style=sysv

sh.elf:
	mkdir -p build
	gcc $(GCCPARAMS) -c sh.c -o build/sh.o
	gcc $(LDPARAMS)  build/sh.o ../../lib/crt/crt.o ../../lib/libc/libc.so -o $@

clean:
	rm -rf build
	rm sh
