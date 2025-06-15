GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions \
			-fno-leading-underscore -fno-stack-protector -Wwrite-strings \
			-Os -ffunction-sections -fdata-sections -mno-sse -msoft-float -finline-small-functions

ASPARAMS = --32
NASMPARAMS = -felf32
LDPARAMS = -melf_i386

SRC_DIRS := asm src src/common src/exec/elf src/exec src/fs/fat16 src/hardware src/memory src/net src/driver src/driver/am79c973 src/driver/ata \
			src/driver/ps2 src/driver/rtl8139 src/driver/sata src/driver/vga
BUILD_DIR := build

C_SOURCES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
ASM_SOURCES := $(wildcard $(addsuffix /*.asm, $(SRC_DIRS)))
S_SOURCES := $(wildcard $(addsuffix /*.s, $(SRC_DIRS)))

C_OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
S_OJECTS := $(patsubst %.s, $(BUILD_DIR)/%.o, $(S_SOURCES))
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS) $(S_OJECTS)

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	nasm $(NASMPARAMS) -o $@ $< 

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	as $(ASPARAMS) -o $@ $< 

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	gcc $(GCCPARAMS) -c $< -o $@

kernel.bin: linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)
	#strip --strip-all kernel.bin

clean:
	rm -rf ./build
	rm kernel.bin
