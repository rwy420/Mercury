
void printf_sys(char* str, int length)
{
	asm volatile("xchg %BX, %BX");
	asm volatile(
		"mov $4, %%eax\n"
		"mov $1, %%ebx\n"
		"mov %0, %%ecx\n"
		"mov %1, %%edx\n"
		"int $0x80\n"
		:
		: "r"(str), "r"(length)
		: "eax", "ebx", "ecx", "edx"
	);
}

void printhex(unsigned char h)
{
	char* foo = "00";
	char* hex = "0123456789ABCDEF";
	foo[0] = hex[(h >> 4) & 0xF];
	foo[1] = hex[h & 0xF];
	printf_sys(foo, 2);
}



int main()
{
	static unsigned char count = 0;
	while(1)
	{
		printf_sys("User task: ", 12);
		printhex(count++);
		printf_sys("\n", 2);
		for(volatile unsigned long i = 0; i < 1000000; i++) for(volatile unsigned long i = 0; i < 1000; i++);
	}
	
	asm volatile(
		"mov $1, %%eax\n"
		"mov $1, %%ebx\n"
		"int $0x80\n"
		:
		:
		: "eax", "edi"
	);
}

