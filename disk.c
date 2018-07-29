/*
 * My implementation of Linux df utility
 * Reposting file system disk space usage
 */

#include "include/inc.h"

void info_for_current_files()
{
	
}

void info_for_all_files()
{
	
}

#define SIZE 80
void get_input(int argc, char *argv[])
{
	char files[argc][SIZE];
	memset(files, 0, sizeof files);

	int j = 0;
	for(int i = argc - 1; i > 0; --i)
	{
		if(argv[i][0] == '-')
			break;

		// TODO Check for existing file

		memcpy(files[j++], argv[i], SIZE);
	}

	if(!j)
		info_for_all_files();
	else
		info_for_current_files(files);
}
#undef SIZE

#define OPTIONS "h::"
int main(int argc, char *argv[])
{
	char option = 0;

	while( (option = getopt(argc, argv, OPTIONS)) != -1)
	{
		switch(option)
		{
			case 'h':
				break;
			case '?':
				user_error("Unknown option %c", option);
				break;
			default:
				break;
		}
	}

	get_input(argc, argv);
	
	return 0;
}

