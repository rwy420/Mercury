GCCPARAMS = -m32 -Iinclude -nostdlib
LDPARAMS = -melf_i386 -emain

init.elf:
	mkdir -p build
	gcc $(GCCPARAMS) -c src/main.c -o build/main.o
	ld $(LDPARAMS) -o $@ build/main.o

clean:
	rm -rf build
	rm init.elf
