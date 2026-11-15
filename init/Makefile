GCCPARAMS = -m32 -Iinclude -I../lib/ -nostdlib -fpie
LDPARAMS = -m32 -pie -nostdlib -Wl,-e_start -Wl,--hash-style=sysv -L../libc.so

init.elf:
	mkdir -p build
	gcc $(GCCPARAMS) -c src/main.c -o build/main.o
	gcc $(LDPARAMS)  build/main.o ../crt/crt.o -lc -o $@

clean:
	rm -rf build
	rm init.elf
