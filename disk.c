/*
 * My implementation of Linux df utility
 * Reposting file system disk space usage
 */

#include "include/inc.h"

#define PATH_SIZE 80
#define MAX_SIZE 15

void get_size_file_system(char *mon, size_t *bs, size_t *ubs, size_t *abs, size_t *pubs)
{
	struct statvfs s_fs;
	if(statvfs(mon, &s_fs))
		sys_error("get_size_file_system: statvfs error");

	*bs = (s_fs.f_bsize / 1000) * s_fs.f_blocks;

	if(*bs)
	{
		*abs = (s_fs.f_bsize / 1000) * s_fs.f_bavail;
		*ubs = *bs - *abs;
		float per = (float)*ubs / (float)*bs;
		*pubs = (size_t)(per * 100);
		if(*ubs && !(*pubs))
			*pubs = 1;
	}
	else
	{
		*abs = 0;
		*ubs = 0;
		*pubs = 0;
	}
}
 
void put_title(char ***fs, char ***mon, char ***bs, char ***used, char ***avail,
			   char***percent, size_t *l, sOptions *myOpt)
{
	char
		**filesystem = NULL, **mountedon = NULL, **bs_str = NULL,
		**used_bs_str = NULL, **avail_bs_str = NULL, **per_used_bs_str = NULL;

	filesystem = xrealloc(filesystem, sizeof *filesystem * (*l + 1));
	bs_str = xrealloc(bs_str, sizeof *bs_str * (*l + 1));
	used_bs_str = xrealloc(used_bs_str, sizeof *used_bs_str * (*l + 1));
	avail_bs_str = xrealloc(avail_bs_str, sizeof *avail_bs_str * (*l + 1));
	per_used_bs_str = xrealloc(per_used_bs_str, sizeof *per_used_bs_str * (*l + 1));
	mountedon = xrealloc(mountedon, sizeof *mountedon * (*l + 1));

	*(filesystem + *l) = xmalloc(strlen("File system") + 1);
	if(myOpt->human_readability)
	{
		*(bs_str + *l) = xmalloc(strlen("Size") + 1);
		*(avail_bs_str + *l) = xmalloc(strlen("Avail") + 1);
	}
	else
	{
		*(bs_str + *l) = xmalloc(strlen("1K-blocks") + 1);
		*(avail_bs_str + *l) = xmalloc(strlen("Availiable") + 1);
	}
	*(used_bs_str + *l) = xmalloc(strlen("Used") + 1);
	*(per_used_bs_str + *l) = xmalloc(strlen("Use%") + 1);
	*(mountedon + *l) = xmalloc(strlen("Mounted on") + 1);

	strcpy(*(filesystem + *l), "File system");
	if(myOpt->human_readability)
	{
		strcpy(*(bs_str + *l), "Size");
		strcpy(*(avail_bs_str + *l), "Avail");
	}
	else
	{
		strcpy(*(bs_str + *l), "1K-blocks");
		strcpy(*(avail_bs_str + *l), "Availiable");
	}
	strcpy(*(used_bs_str + *l), "Used");
	strcpy(*(per_used_bs_str + *l), "Use%");
	strcpy(*(mountedon + *l), "Mounted on");

	*l += 1;

	*fs = filesystem;
	*mon = mountedon;
	*bs = bs_str;
	*used = used_bs_str;
	*avail = avail_bs_str;
	*percent = per_used_bs_str;
}

void get_info_mounted_file_systems(char ***fs, char ***mon, size_t *line, char ***bs,
								   char ***ubs, char ***abs, char ***pubs, sOptions *myOpt)
{
	char
		**filesystem = NULL, **mountedon = NULL, **bs_str = NULL,
		**used_bs_str = NULL, **avail_bs_str = NULL, **per_used_bs_str = NULL;
	size_t block_size = 0, used_bs = 0, avail_bs = 0, per_used_bs = 0;
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp)
		sys_error("info_for_all_files error: fopen error");

	size_t l = 0;

	put_title(&filesystem, &mountedon, &bs_str, &used_bs_str, &avail_bs_str,
			  &per_used_bs_str, &l, myOpt);

	char temp[2][PATH_SIZE] = {0};
	while(fscanf(fp, "%s %s", temp[0], temp[1]))
	{
		if(!(temp[0][0]))
			break;

		char temp_bs_str[MAX_SIZE] = {0};
		char temp_used_bs_str[MAX_SIZE] = {0};
		char temp_avail_bs_str[MAX_SIZE] = {0};
		char temp_used_perbs_str[MAX_SIZE] = {0};
		get_size_file_system(temp[1], &block_size, &used_bs, &avail_bs, &per_used_bs);

		if(block_size && myOpt->human_readability)
		{
			if(block_size >= 1000000)
			{
				float human_bs = (float)block_size / 1000000;
				float human_ubs = (float)used_bs / 1000000;
				float human_abs = (float)avail_bs / 1000000;
				snprintf(temp_bs_str, MAX_SIZE, "%0.1fG", human_bs);
				snprintf(temp_used_bs_str, MAX_SIZE, "%0.1fG", human_ubs);
				snprintf(temp_avail_bs_str, MAX_SIZE, "%0.1fG", human_abs);
			}
			else if(block_size >= 1000)
			{
				float human_bs = (float)block_size / 1000;
				float human_ubs = (float)used_bs / 1000;
				float human_abs = (float)avail_bs / 1000;
				snprintf(temp_bs_str, MAX_SIZE, "%0.1fM", human_bs);
				snprintf(temp_used_bs_str, MAX_SIZE, "%0.1fM", human_ubs);
				snprintf(temp_avail_bs_str, MAX_SIZE, "%0.1fM", human_abs);
			}
			else
			{
				snprintf(temp_bs_str, MAX_SIZE, "%ldK", block_size);
				snprintf(temp_used_bs_str, MAX_SIZE, "%ldK", used_bs);
				snprintf(temp_avail_bs_str, MAX_SIZE, "%ldK", avail_bs);
			}

			snprintf(temp_used_perbs_str, MAX_SIZE, "%ld%c", per_used_bs, 37);
		}
		else if(block_size)
		{
			snprintf(temp_bs_str, MAX_SIZE, "%ld", block_size);
			snprintf(temp_used_bs_str, MAX_SIZE, "%ld", used_bs);
			snprintf(temp_avail_bs_str, MAX_SIZE, "%ld", avail_bs);
			snprintf(temp_used_perbs_str, MAX_SIZE, "%ld%c", per_used_bs, 37);
		}
		else
		{
			snprintf(temp_bs_str, 2, "0");
			snprintf(temp_used_bs_str, 2, "0");
			snprintf(temp_avail_bs_str, 2, "0");
			snprintf(temp_used_perbs_str, 3, "0%c", 37);
		}

		if(block_size || myOpt->print_all_mounts)
		{
			filesystem = xrealloc(filesystem, sizeof *filesystem * (l + 1));
			bs_str = xrealloc(bs_str, sizeof *bs_str * (l + 1));
			used_bs_str = xrealloc(used_bs_str, sizeof *used_bs_str * (l + 1));
			avail_bs_str = xrealloc(avail_bs_str, sizeof *avail_bs_str * (l + 1));
			per_used_bs_str = xrealloc(per_used_bs_str, sizeof *per_used_bs_str * (l + 1));
			mountedon = xrealloc(mountedon, sizeof *mountedon * (l + 1));

			*(filesystem + l) = xmalloc(strlen(temp[0]) + 1);
			*(bs_str + l) = xmalloc(strlen(temp_bs_str) + 1);
			*(used_bs_str + l) = xmalloc(strlen(temp_used_bs_str) + 1);
			*(avail_bs_str + l) = xmalloc(strlen(temp_avail_bs_str) + 1);
			*(per_used_bs_str + l) = xmalloc(strlen(temp_used_perbs_str) + 1);
			*(mountedon + l) = xmalloc(strlen(temp[1]) + 1);

			strcpy(*(filesystem + l), temp[0]);
			strcpy(*(bs_str + l), temp_bs_str);
			strcpy(*(used_bs_str + l), temp_used_bs_str);
			strcpy(*(avail_bs_str + l), temp_avail_bs_str);
			strcpy(*(per_used_bs_str + l), temp_used_perbs_str);
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
	*ubs = used_bs_str;
	*abs = avail_bs_str;
	*pubs = per_used_bs_str;
	*mon = mountedon;
	*line = l;
}

void colomn_width(size_t num, sOptions *myOpt,
				  char *fs[], char fs_b[][PATH_SIZE],
				  char *bs[], char bs_b[][MAX_SIZE],
				  char *ubs[], char ubs_b[][MAX_SIZE],
				  char *abs[], char abs_b[][MAX_SIZE],
				  char *pubs[], char pubs_b[][MAX_SIZE])
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

	size_t max_width_ubs = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t width = strlen(ubs[i]);
		if(width > max_width_ubs)
			max_width_ubs = width;
	}
	for(size_t i = 0; i < num; ++i)
		for(size_t b = max_width_ubs - strlen(ubs[i]), j = 0; b > 0; --b, ++j)
			ubs_b[i][j] = ' ';

	size_t max_width_abs = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t width = strlen(abs[i]);
		if(width > max_width_abs)
			max_width_abs = width;
	}
	for(size_t i = 0; i < num; ++i)
		for(size_t b = max_width_abs - strlen(abs[i]), j = 0; b > 0; --b, ++j)
			abs_b[i][j] = ' ';

	size_t max_width_pubs = 0;
	for(size_t i = 0; i < num; ++i)
	{
		size_t width = strlen(pubs[i]);
		if(width > max_width_pubs)
			max_width_pubs = width;
	}
	for(size_t i = 0; i < num; ++i)
		for(size_t b = max_width_pubs - strlen(pubs[i]), j = 0; b > 0; --b, ++j)
			pubs_b[i][j] = ' ';
}

void get_size_fs(char fs[][PATH_SIZE], char mon[][PATH_SIZE], size_t num,
				 char ***bs, char ***ubs, char ***abs, char ***pubs, sOptions *myOpt)
{
	for(size_t i = 0; i < num; ++i)
	{
		struct statvfs s_fs;
		if(statvfs(mon[i], &s_fs))
			sys_error("get_size_fs: statvfs error");
		printf("%s: %ld - %ld - %ld\n", fs[i], s_fs.f_bsize, s_fs.f_blocks, s_fs.f_bfree);
	}
}

void find_mounted_path(const char files[][PATH_SIZE], char mon[][PATH_SIZE], size_t num)
{
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp) sys_error("find_mounted_path: fopen error");
	char temp[2][PATH_SIZE] = {0};
	
	for(size_t i = 0; i < num;)
	{
		fscanf(fp, "%s %s", temp[0], temp[1]);
		if(!temp[0]) break;

		if(!strcmp(files[i], temp[0]))
			memcpy(mon[i++], temp[1], PATH_SIZE);

		char c = 0;
		while( (c = fgetc(fp)) != '\n');
	}
}

void info_for_current_files(char files[][PATH_SIZE], size_t num, sOptions *myOpt)
{
	char mountedon[num][PATH_SIZE];
	memset(mountedon, 0, sizeof mountedon);
	char **block_size = NULL, **used_bs = NULL, **avail_bs = NULL, **per_bs = NULL;

//	find_mounted_path(files, mountedon, num);
	get_size_fs(files, mountedon, num, &block_size, &used_bs, &avail_bs, &per_bs, myOpt);
}

void info_for_all_files(sOptions *myOpt)
{
	char **filesystem = NULL, **mountedon = NULL;
	size_t amount = 0;
	char **block_size = NULL, **used_bs = NULL, **avail_bs = NULL, **per_bs = NULL;

	get_info_mounted_file_systems(&filesystem, &mountedon, &amount, &block_size,
								  &used_bs, &avail_bs, &per_bs, myOpt);

	char fs_b[amount][PATH_SIZE];
	memset(fs_b, 0, sizeof fs_b);
	char bs_b[amount][MAX_SIZE];
	memset(bs_b, 0, sizeof bs_b);
	char ubs_b[amount][MAX_SIZE];
	memset(ubs_b, 0, sizeof ubs_b);
	char abs_b[amount][MAX_SIZE];
	memset(abs_b, 0, sizeof abs_b);
	char pubs_b[amount][MAX_SIZE];
	memset(pubs_b, 0, sizeof pubs_b);

	colomn_width(amount, myOpt, filesystem, fs_b, block_size, bs_b,
				 used_bs, ubs_b, avail_bs, abs_b, per_bs, pubs_b);

	for(size_t i = 0; i < amount; ++i)
		printf("%s%s  %s%s  %s%s %s%s %s%s %s\n",
			   filesystem[i], fs_b[i],
			   bs_b[i], block_size[i],
			   ubs_b[i], used_bs[i],
			   abs_b[i], avail_bs[i],
			   pubs_b[i], per_bs[i],
			   mountedon[i]);

	for(size_t i = 0; i < amount; ++i)
	{
		free(*(filesystem + i));
		free(*(block_size + i));
		free(*(used_bs + i));
		free(*(avail_bs + i));
		free(*(per_bs + i));
		free(*(mountedon + i));
	}
	free(filesystem);
	free(block_size);
	free(used_bs);
	free(avail_bs);
	free(per_bs);
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

	size_t f_num = 0;
	for(int i = argc - 1; i > 0; --i)
	{
		if(argv[i][0] == '-')
			break;

		if(check_existence_file(argv[i]))
			memcpy(files[f_num++], argv[i], PATH_SIZE);
		else 
			log_error("File does not exist: \"%s\"\n", argv[i]);
	}

	if(!f_num)
		info_for_all_files(myOpt);
	else
		info_for_current_files(files, f_num, myOpt);
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

