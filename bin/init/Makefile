GCCPARAMS = -m32 -Iinclude -I../lib/libc -nostdlib -nostdinc -ffreestanding -fpie
LDPARAMS = -m32 -pie -nostdlib -nostdinc -I../lib/libc -fno-builtin -nostartfiles -ffreestanding -Wl,-y,write \
		   -Wl,-y,_start -Wl,-e_start -Wl,--hash-style=sysv

init.elf:
	mkdir -p build
	gcc $(GCCPARAMS) -c src/main.c -o build/main.o
	gcc $(LDPARAMS)  build/main.o ../lib/crt/crt.o ../libc.so -o $@

clean:
	rm -rf build
	rm init.elf
