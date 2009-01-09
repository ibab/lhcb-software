#ifdef __cplusplus

extern "C" {
#endif

#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define DELIMITER '$'
#define DELIMITER_CHAR "$"

