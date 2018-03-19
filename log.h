#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "fila.h"

#define	SUCESS		1
#define FAILURE		0	
#define LOG_BASEPATH		"./logDir"

char fileName[] = "cardco.log";
char pathFile[128];
unsigned char logBuf[512];
extern int errno;
char *sname, *dname;
int fsrce, fdest,fout;
int bytesrd, byteswr;

