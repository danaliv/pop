#ifndef __FGETLN_H__
#define __FGETLN_H__

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#if !defined(BSD) && !defined(__APPLE__)
#include <bsd/stdio.h>
#endif

#endif
