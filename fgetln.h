#ifndef FGETLN_H
#define FGETLN_H

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#if !defined(BSD) && !defined(__APPLE__)
#include <bsd/stdio.h>
#endif

#endif // FGETLN_H
