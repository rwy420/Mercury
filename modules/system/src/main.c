// system_call.h
static inline int syscall(int syscall_number, int arg1, int arg2, int arg3) {
    int result;
    asm volatile (
        "int $0x80"             // Software interrupt for system calls
        : "=a"(result)          // Output in EAX
        : "a"(syscall_number),  // System call number in EAX
          "b"(arg1),            // First argument in EBX
          "c"(arg2),            // Second argument in ECX
          "d"(arg3)             // Third argument in EDX
        : "memory"
    );
    return result;
}


void print()
{
	char* msg = "System module started";
	//asm("int $0x80" : : "a" (4), "b" (msg));
	syscall(0x4, (unsigned long) msg, 0, 0);
}

int main()
{
	print();
}
