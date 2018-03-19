/*
** 
**
*/

#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <limits.h>

#include "cardco.h"

#define MAXSLOTS 500

extern int errno;

key_t chave;
int	shmid;
struct listaEventos {
	int nroslot;
	int key;
	int busy;
	time_t timeStamp;
	int lock;
	


};
struct cardco	*shmptr;
struct listaEventos	*gp;
int	*posptr;

struct semid_ds semid_ds;
struct sembuf	*sops;
union semun {
     int val;
     struct semid_ds * buf;
     ushort * array;
};


FILE *fdbg;

static char dbg_path[80] = "./debug/areacomp.debug";
static char cfg_path[80] = "./cfg/areacomp.cfg";
static char shmkey_path[80] = "./AreaComp";
static char semkey_path[80];

struct cardco *localizaEvento(int key);
void liberaEvento(int key);
struct cardco *obtemSlotLivre(int key);
struct cardco *obtemSlotForced(int key);
void gravaEvento(struct cardco *cardcomsg);
time_t stamp ();


