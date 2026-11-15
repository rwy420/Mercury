#include <printf.h>
#include <common/screen.h>
#include <vesa.h>

extern VesaInfo g_vesa_info;

void uint8_t_to_string(uint8_t value, char* buffer)
{
	int i = 0;
	do
	{
		buffer[i++] = '0' + (value % 10);
		value /= 10;
	} while(value > 0);
	buffer[i] = '\0';

	for(int j = 0, k = i - 1; j < k; j++, k--)
	{
		char temp = buffer[j];
		buffer[j] = buffer[k];
		buffer[k] = temp;
	}
}

void uint32_t_to_string(uint32_t value, char *buffer)
{
    char temp[11];
    int i = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }

    buffer[j] = '\0';
}
