#ifndef __FILE_LOADED
#define __FILE_LOADED   1

#ifdef _WIN32
#include <io.h>
/*      FILE - V3.0     */

/*
        Manifest constants used by BSD 4.2 OPEN function
 */

# define O_RDONLY       000
# define O_WRONLY       001
# define O_RDWR         002
# define O_NDELAY       004
# define O_NOWAIT       004
# define O_APPEND       010
# define O_CREAT        01000
# define O_TRUNC        02000
# define O_EXCL         04000

#else
#include <unistd.h>
#include <fcntl.h>
#endif

#endif                                  /* __FILE_LOADED */
