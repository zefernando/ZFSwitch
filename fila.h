#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <limits.h>

#define FORMINQ         1
#define FORMOUTQ        2
#define AUTHQ           3
#define DRIVERQ         4
#define LOGQ            5
#define FUPDQ		6
#define PAYBAL		7
#define MAXMSGLEN	2048
struct fila {
        int tipo;
        char buf[MAXMSGLEN];
};
