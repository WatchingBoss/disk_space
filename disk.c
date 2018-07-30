/*
 * My implementation of Linux df utility
 * Reposting file system disk space usage
 */

#include "include/inc.h"

#define PATH_SIZE 80

/*
 * We read file /proc/mounts to get decives which are mount to system
 * Go to /sys/class/block/'name'/'subname if required'/size -> number of 512 byte blocks = N
 * Easy convert to 1K-blocks: N / 2
 */

void info_for_current_files(char files[][PATH_SIZE], int num, sOptions *myOpt)
{
	
}

void get_mounted_file_systems(char ***fs, char ***mon, size_t *line)
{
	char **filesystem = NULL, **mountedon = NULL;
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp)
		sys_error("info_for_all_files error: fopen error");

	char temp[2][PATH_SIZE] = {0};
	size_t l = 0;
	while(fscanf(fp, "%s", temp[0]) &&
		  fscanf(fp, "%s", temp[1]))
	{
		if(!(temp[0][0]))
			break;

		filesystem = xrealloc(filesystem, sizeof *filesystem * (l + 1));
		mountedon = xrealloc(mountedon, sizeof *mountedon * (l + 1));
		*(filesystem + l) = xmalloc(strlen(temp[0]) + 1);
		*(mountedon + l) = xmalloc(strlen(temp[1]) + 1);

		strcpy(*(filesystem + l), temp[0]);
		strcpy(*(mountedon + l), temp[1]);

		l++;

		char c = 0;
		while( (c = fgetc(fp)) != '\n') {}
		memset(temp, 0, sizeof temp);
	}
	fclose(fp);

	*fs = filesystem;
	*mon = mountedon;
	*line = l;
}

void colomn_width(size_t num, char *fs[], char fs_b[][PATH_SIZE])
{
	size_t max_width_fs = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t width = strlen(fs[i]);
		if(width > max_width_fs)
			max_width_fs = width;
	}

	for(size_t i = 0; i < num; ++i)
		for(size_t b = max_width_fs - strlen(fs[i]), j = 0; b > 0; --b, ++j)
			fs_b[i][j] = ' ';
}

void info_for_all_files(sOptions *myOpt)
{
	char **filesystem = NULL, **mountedon = NULL;
	size_t amount = 0;

	get_mounted_file_systems(&filesystem, &mountedon, &amount);

	char fs_b[amount][PATH_SIZE];
	memset(fs_b, 0, sizeof fs_b);
	colomn_width(amount, filesystem, fs_b);
	
	for(size_t i = 0; i < amount; ++i)
		printf("%s%s %s\n",
			   filesystem[i], fs_b[i],
			   mountedon[i]);
	
	for(size_t i = 0; i < amount; ++i)
	{
		free(*(filesystem + i));
		free(*(mountedon + i));
	}
	free(filesystem);
	free(mountedon);
}

int check_existence_file(const char *file)
{
	struct stat st;
	if(stat(file, &st))
		return 0;
	return 1;
}

void get_input(int argc, char *argv[], sOptions *myOpt)
{
	char files[argc - 1][PATH_SIZE];
	memset(files, 0, sizeof files);

	int j = 0;
	for(int i = argc - 1; i > 0; --i)
	{
		if(argv[i][0] == '-')
			break;

		if(check_existence_file(argv[i]))
			memcpy(files[j++], argv[i], PATH_SIZE);
		else 
			log_error("File does not exist: \"%s\"\n", argv[i]);
	}

	if(!j)
		info_for_all_files(myOpt);
	else
		info_for_current_files(files, j, myOpt);
}

#define OPTIONS "h::"
int main(int argc, char *argv[])
{
	char option = 0;
	sOptions myOpt;
	myOpt.print_all_mounts = false;
	myOpt.human_readability = false;

	while( (option = getopt(argc, argv, OPTIONS)) != -1)
	{
		switch(option)
		{
			case 'h':
				myOpt.human_readability = true;
				break;
			case 'a':
				myOpt.print_all_mounts = true;
				break;
			case '?':
				user_error("Unknown option %c", option);
				break;
			default:
				break;
		}
	}

	get_input(argc, argv, &myOpt);
	
	return 0;
}
#undef OPTIONS

