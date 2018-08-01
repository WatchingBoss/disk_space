/*
 * My implementation of Linux df utility
 * Reposting file system disk space usage
 */

#include "include/inc.h"

#define PATH_SIZE 80
#define MAX_SIZE 15

/*
 * We read file /proc/mounts to get decives which are mount to system
 * Go to /sys/class/block/'name'/'subname if required'/size -> number of 512 byte blocks = N
 * Easy convert to 1K-blocks: N / 2
 */

void info_for_current_files(char files[][PATH_SIZE], int num, sOptions *myOpt)
{
	
}

size_t get_size_file_system(char *fs)
{
	size_t block_size = 0;
	char path[PATH_SIZE] = {0};

//	struct statvfs fs_stat;

	if(fs[0] == '/' && fs[1] == 'd')
	{
		int fd = open(fs, O_RDONLY);
		if(fd < 0) sys_error("get_size_file_system: open");
		unsigned long int size = 0;
		if(ioctl(fd, BLKGETSIZE, &size) < 0) sys_error("get_size_file_system: ioctl");
		size /= 2;
		printf("%s: %ld\n", fs, size);
		close(fd);

		char temp_t[PATH_SIZE] = {0};
		memcpy(temp_t, &(fs[5]), PATH_SIZE);

		snprintf(path, PATH_SIZE, "/sys/class/block/%s/size", temp_t);

		FILE *fp = fopen(path, "r");
		if(!fp)
			sys_error("get_size_file_system: fopen error");

		fscanf(fp, "%ld", &block_size);

		block_size /= 2;
	}

	return block_size;
}

void get_mounted_file_systems(char ***fs, char ***mon, size_t *line, char ***bs,
							  sOptions *myOpt)
{
	char **filesystem = NULL, **mountedon = NULL, **bs_str = NULL;
	size_t block_size = 0;
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp)
		sys_error("info_for_all_files error: fopen error");

	size_t l = 0;
	filesystem = xrealloc(filesystem, sizeof *filesystem * (l + 1));
	bs_str = xrealloc(bs_str, sizeof *bs_str * (l + 1));
	mountedon = xrealloc(mountedon, sizeof *mountedon * (l + 1));
	*(filesystem + l) = xmalloc(strlen("File system") + 1);
	if(myOpt->human_readability)
		*(bs_str + l) = xmalloc(strlen("Size") + 1);
	else
		*(bs_str + l) = xmalloc(strlen("1K-blocks") + 1);
	*(mountedon + l) = xmalloc(strlen("Mounted on") + 1);
	strcpy(*(filesystem + l), "File system");
	if(myOpt->human_readability)
		strcpy(*(bs_str + l), "Size");
	else
		strcpy(*(bs_str + l), "1K-blocks");
	strcpy(*(mountedon + l), "Mounted on");
	++l;

	char temp[2][PATH_SIZE] = {0};
	while(fscanf(fp, "%s", temp[0]) &&
		  fscanf(fp, "%s", temp[1]))
	{
		if(!(temp[0][0]))
			break;

		char temp_bs_str[MAX_SIZE] = {0};
		block_size = get_size_file_system(temp[0]);

		if(block_size && myOpt->human_readability)
		{
			if(block_size >= 1000000)
			{
				float human_bs = (float)block_size / 1000000;
				snprintf(temp_bs_str, MAX_SIZE, "%0.1fG", human_bs);
			}
			else if(block_size >= 1000)
			{
				float human_bs = (float)block_size / 1000;
				snprintf(temp_bs_str, MAX_SIZE, "%0.1fM", human_bs);
			}
			else
				snprintf(temp_bs_str, MAX_SIZE, "%ldK", block_size);
		}
		else if(block_size)
			snprintf(temp_bs_str, MAX_SIZE, "%ld", block_size);
		else
			snprintf(temp_bs_str, 2, "0");

		if(block_size || myOpt->print_all_mounts)
		{
			filesystem = xrealloc(filesystem, sizeof *filesystem * (l + 1));
			bs_str = xrealloc(bs_str, sizeof *bs_str * (l + 1));
			mountedon = xrealloc(mountedon, sizeof *mountedon * (l + 1));

			*(filesystem + l) = xmalloc(strlen(temp[0]) + 1);
			*(bs_str + l) = xmalloc(strlen(temp_bs_str) + 1);
			*(mountedon + l) = xmalloc(strlen(temp[1]) + 1);

			strcpy(*(filesystem + l), temp[0]);
			strcpy(*(bs_str + l), temp_bs_str);
			strcpy(*(mountedon + l), temp[1]);
			
			l++;
		}

		char c = 0;
		while( (c = fgetc(fp)) != '\n');
		memset(temp, 0, sizeof temp);
	}
	fclose(fp);

	*fs = filesystem;
	*bs = bs_str;
	*mon = mountedon;
	*line = l;
}

void colomn_width(size_t num, sOptions *myOpt,
				  char *fs[], char fs_b[][PATH_SIZE],
				  char *bs[], char bs_b[][MAX_SIZE])
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

	size_t max_width_bs = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t width = strlen(bs[i]);
		if(width > max_width_bs)
			max_width_bs = width;
	}
	for(size_t i = 0; i < num; ++i)
		for(size_t b = max_width_bs - strlen(bs[i]), j = 0; b > 0; --b, ++j)
			bs_b[i][j] = ' ';
}

void info_for_all_files(sOptions *myOpt)
{
	char **filesystem = NULL, **mountedon = NULL;
	size_t amount = 0;
	char **block_size = NULL;

	get_mounted_file_systems(&filesystem, &mountedon, &amount, &block_size, myOpt);

	char fs_b[amount][PATH_SIZE];
	memset(fs_b, 0, sizeof fs_b);

	char bs_b[amount][MAX_SIZE];
	memset(bs_b, 0, sizeof bs_b);

	colomn_width(amount, myOpt,
				 filesystem, fs_b,
				 block_size, bs_b);

	for(size_t i = 0; i < amount; ++i)
		printf("%s%s  %s%s  %s\n",
			   filesystem[i], fs_b[i],
			   block_size[i], bs_b[i],
			   mountedon[i]);
	

	for(size_t i = 0; i < amount; ++i)
	{
		free(*(filesystem + i));
		free(*(block_size + i));
		free(*(mountedon + i));
	}
	free(filesystem);
	free(block_size);
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

