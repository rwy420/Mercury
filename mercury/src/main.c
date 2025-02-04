#include "qslibc/qs_log.h"
#include "drivers/vga.h"

int main()
{
	kscreen_clear();
	kprintf("           __  __                                 ___  ____ \n");
	kprintf("          |  \\/  | ___ _ __ ___ _   _ _ __ _   _ / _ \\/ ___| \n");
	kprintf("          | |\\/| |/ _ \\ '__/ __| | | | '__| | | | | | \\___ \\ \n");
	kprintf("          | |  | |  __/ | | (__| |_| | |  | |_| | |_| |___) | \n");
	kprintf("          |_|  |_|\\___|_|  \\___|\\__,_|_|   \\__, |\\___/|____/ \n"); 
	kprintf("                                           |___/ \n");

	//vga_init();

	while(1);
}
