/*
 * Helpful functions 
 */

#include "include/inc.h"

void * xrealloc(void *ptr, size_t bytes)
{
	if( !(ptr = realloc(ptr, bytes)))
		perror("realloc in xrealloc error");
	return ptr;
}

void * xmalloc(size_t bytes)
{
	void *ptr = malloc(bytes);
	if(!ptr)
		perror("malloc in xmalloc error");
	return ptr;
}

void user_error(char *e, ...)
{
	va_list args;
	va_start(args, e);
	vprintf(e, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

void sys_error(char *e)
{
	perror(e);
	exit(EXIT_FAILURE);
}




