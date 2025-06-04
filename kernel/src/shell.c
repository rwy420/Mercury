#include <shell.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/mem_manager.h>
#include <fs/fat16/fat16.h>
#include <exec/elf/elf_loader.h>

char input_buffer[0xFF];
uint32_t input_buffer_pointer;

static void parse_command(char* command);
static char* get_argument(char* command, int idx);

void shell_init()
{
	memset(&input_buffer, 0, 0xFF);
	input_buffer_pointer = 0;
	printf("# ");
}

void shell_putc(uint8_t key)
{
	if(key == '\b')
	{
		input_buffer_pointer--;
		terminal_move_left();
		return;
	}

	char* txt = " ";
	txt[0] = key;
	input_buffer[input_buffer_pointer++] = key;
	printf(txt);
}

void shell_exec()
{
	for(int i = 0; i < 0xFF; i++)
	{
		if(input_buffer[i] == ' ') input_buffer[i] = '\0';
	}

	parse_command(input_buffer);
	memset(&input_buffer, 0, 0xFF);
	input_buffer_pointer = 0;
	printf("# ");
}

static void parse_command(char* command)
{
	printf("\n");

	if(strcmp(command, "ls") == 0)
	{
		char* arg1 = get_argument(input_buffer, 1);
		if(arg1[0] == '\0') arg1[0] = '/';

		char name[11];
		uint32_t i = 0;
		while(fat16_ls(&i, name, arg1) == 1)
		{
			printf(name);
			printf("\n");
		}
		return;
	}
	else if(strcmp(command, "clear") == 0)
	{
		clear_screen();
		return;
	}
	else if(strcmp(command, "elfexec") == 0)
	{
		char* arg1 = get_argument(input_buffer, 1);
		uint32_t file_size = fat16_size(arg1);
		int fd = fat16_open(arg1, 'r');

		if(fd == -1)
		{
			printf("File not found\n");
			return;
		}

		char* buffer = kmalloc(file_size);
		fat16_read(fd, buffer, file_size);
		void(*entry)();
		entry = image_load(buffer, file_size, false);
		kfree(buffer);

		entry();
	}


	printf("Command not found\n");
}

static char* get_argument(char* command, int idx)
{
	int argument_idx = 0;

	for(int i = 0; i < 0xFF; i++)
	{
		if(command[i] == '\0')
		{
			argument_idx++;
		}

		if(argument_idx == idx) return (char*) (command + i + 1);
	}

	return (char*) '\0';
}
