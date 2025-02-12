#include <fs/fat16/fat16_path.h>
#include <memory/common.h>

static bool is_character_valid(char c)
{
	return ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9');
}

int to_short_filename(char* short_filename, const char* long_filename)
{
	uint8_t i = 0;
	uint8_t sep = 0;

	if(long_filename[0] == '/') long_filename++;

	if(long_filename[0] == '\0') return -1;

	for(i = 0; i < 9; i++)
	{
		if(long_filename[i] == '\0')
		{
			memset(&short_filename[i], ' ', 11 - i);
			return 0;
		}

		if(long_filename[i] == '.')
		{
			sep = i;
			break;
		}

		if(!is_character_valid(long_filename[i])) return -1;

		short_filename[i] = long_filename[i];
	}

	if(i == 9) return -1;

	memset(&short_filename[sep], ' ', 8 - sep);

	for(i = 0; i < 3; i++)
	{
		if(long_filename[sep + 1 + i] == '\0')
		{
			break;
		}

		if(!is_character_valid(long_filename[sep + 1 + i])) return -1;

		short_filename[8 + i] = long_filename[sep + 1 + i];
	}

	if(i == 3 && long_filename[sep + 4] != '\0') return -1;

	memset(&short_filename[8 + i], ' ', 3 - i);

	return 0;
}

int get_subdir(char* subdir_name, uint16_t* index, const char* path)
{
	const uint16_t beg = *index;
	uint32_t len = 0;

	if(path[beg] != '/')
		return -1;

	len++;
	while(path[beg + len] != '\0')
	{
		if(path[beg + len] == '/') break;

		len++;
	}

	if(path[beg + len] != '/')
		return -2;

	memcpy(subdir_name, &path[beg], len);
	subdir_name[len] = '\0';

	*index = beg + len;
	return 0;
}

bool is_in_root(const char* path)
{
	char subdir_name[MAX_FILENAME_LEN + 1];
	uint16_t index = 0;

	return get_subdir(subdir_name, &index, path) < 0;
}
