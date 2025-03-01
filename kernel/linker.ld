OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)
ENTRY(k_entry)

SECTIONS
{
	. = 1M;
	kernel_start = 1M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text*)
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data*)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
		*stack(.bss)
	}

	/DISCARD/  :
	{
		*(.fini_array*)
		*(.comment)
	}

	kernel_end = .;
}
