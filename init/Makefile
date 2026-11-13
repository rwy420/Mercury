GCCPARAMS = -m32 -Iinclude -nostdlib -fpie
LDPARAMS = -melf_i386 -pie -emain

init.elf:
	mkdir -p build
	gcc $(GCCPARAMS) -c src/main.c -o build/main.o
	ld $(LDPARAMS) -o $@ build/main.o

clean:
	rm -rf build
	rm init.elf
