#ifndef DISK_H
#define DISK_H

#include "inc.h"

#define PATH_SIZE 80
#define MAX_SIZE 15

typedef struct sOptions
{
	bool print_all_mounts;
	bool human_readability;
} sOptions;

extern void put_title(char ***, char ***, char ***, char ***, char ***,
					  char***, size_t *, sOptions *);

extern void get_size_file_system(char *, size_t *, size_t *, size_t *, size_t *);
extern void get_info_mounted_file_systems(char ***, char ***, size_t *, char ***,
										  char ***, char ***, char ***, sOptions *,
										  char [][PATH_SIZE]);
extern void colomn_width(size_t , sOptions *,
						 char *[], char [][PATH_SIZE],
						 char *[], char [][MAX_SIZE],
						 char *[], char [][MAX_SIZE],
						 char *[], char [][MAX_SIZE],
						 char *[], char [][MAX_SIZE]);

extern void free_memory(char ***, char ***, char ***, char ***,
						char ***, char ***, size_t);
extern void printing_col(char **, char **, char **, char **,
						 char **, char **, size_t, sOptions *);

extern void info_for_current_files(char [][PATH_SIZE], size_t, sOptions *);
extern void info_for_all_files(sOptions *);

extern void get_input(int , char *[], sOptions *);

extern int check_existence_file(const char *);

#endif

