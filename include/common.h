#ifndef COMMON_H
#define COMMON_H

#include "inc.h"

extern void * xrealloc(void *, size_t);
extern void * xmalloc(size_t);

extern void log_error(char *, ...);
extern void user_error(char *, ...);
extern void sys_error(char *);

#endif
