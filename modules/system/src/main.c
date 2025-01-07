void print()
{
	char* msg = "System module started";
	asm("int $0x80" : : "a" (4), "b" (msg));
}

int main()
{
	print();
}
