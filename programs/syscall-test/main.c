char msg[] = "Hello world from ELF32 in FAT32";
unsigned int len = sizeof(msg) - 1;



int main()
{
	asm volatile(
		"mov $4, %%eax\n"
		"mov $1, %%ebx\n"
		"mov %0, %%ecx\n"
		"mov %1, %%edx\n"
		"int $0x80\n"
		:
		: "r"(msg), "r"(len)
		: "eax", "ebx", "ecx", "edx"
	);

	asm volatile(
		"mov $1, %%eax\n"
		"mov $1, %%ebx\n"
		"int $0x80\n"
		:
		:
		: "eax", "edi"
	);
}
