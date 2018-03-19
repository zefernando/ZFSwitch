
#include "storage.h"

/*
*	Cria a area de memória compartilhada. 
*/

int	criaStorage()
{
	int	flg;
	char	*path;
	struct cardco *C_ptr;
	int segsize;

	if((path = getenv("FEB_DEBUG" )) == (char *) NULL)
		path = dbg_path;

	/* Abrea o Arquivo de debug da area compartilhada */
	fdbg = fopen(path,"a");

	/*
	 *      Initialise
	 */

	if((path = getenv("FEB_SHMKEY" )) == (char *) NULL)
		path = shmkey_path;
	chave = ftok(path, 1);
	if(chave == -1) return -1;
	fprintf(fdbg,"Chave %x\n", chave);
	segsize = MAXSLOTS * sizeof(struct listaEventos) + MAXSLOTS * sizeof(struct cardco); 

	if ((shmid = shmget(chave, segsize, SHM_R  | (SHM_R >>3) | SHM_W )) < 0) {
		fprintf(fdbg,"shmget nao localizou id\n");
		if ((shmid = shmget(chave, segsize, SHM_R | (SHM_R >> 3) | SHM_W  | IPC_CREAT | IPC_EXCL)) < 0) {

			fprintf(fdbg,"shmget nao conseguiu criar id\n");
			exit(0);

		}

	}
	if ((posptr = (int *) shmat(shmid, 0, 0)) == (void *) -1) {
		fprintf(fdbg,"shmat\n");
		return -1;
	}
	shmptr = (struct cardco *)(posptr + sizeof(struct listaEventos));
	gp	= (struct listaEventos *) (posptr);
	/* posptr contém o numero de slots de centrais na memoria compartilhada */
	C_ptr = shmptr;

	return 0;
}

int	obtemStorage()
{
	int	segsize;
	
	char	*path;
	struct cardco *C_ptr;

	if((path = getenv("FEB_DEBUG" )) == (char *) NULL)
		path = dbg_path;

	/* Abrea o Arquivo de debug da area compartilhada */
	fdbg = fopen(path,"a");

	/*
	 *      Initialise
	 */

	if((path = getenv("FEB_SHMKEY" )) == (char *) NULL)
		path = shmkey_path;
	chave = ftok(path, 1);
	if(chave == -1) return -1;
	fprintf(fdbg,"Chave %x\n", chave);
	segsize = MAXSLOTS * sizeof(struct listaEventos) + MAXSLOTS * sizeof(struct cardco);

	if ((shmid = shmget(segsize, segsize, SHM_R | (SHM_R >>3) | SHM_W )) < 0) {
		fprintf(fdbg,"shmget nao localizou id\n");

		return (-1);
	}
	if ((posptr = (int *) shmat(shmid, 0, 0)) == (void *) -1) {
		fprintf(fdbg,"shmat\n");
		return -1;
	}
	shmptr = (struct cardco *)(posptr + 500*sizeof(struct listaEventos));
	gp	= (struct listaEventos *) (posptr);
	/* posptr contém o numero de slots de centrais na memoria compartilhada */
	C_ptr = shmptr;
	return 0;
}







int removeStorage()
{
	if(shmctl(shmid, IPC_RMID, 0 ) < 0 ) {
		fprintf(fdbg,"Nao consegui remover a area de memoria compartilhada\n");
		exit(0);
	}

	return 0;
}



/*
**  Cria chaves para os eventos usados pelo sistema:
**  As chaves sao criadas pela concatenacao de 3 campos:
**
**	XXXXYYYYZZ
**      |   |   |
**      |   |   |----> Campo 3	(2 digitos)
**      |   |--------> Campo 2	(4 digitos)
**      |------------> Campo 1	(4 digitos)
**
**  Nas mensagens RVA3:
**			Campo1 = Banco (Se Banco > 9000 entao Banco - 8000);
**			Campo2 = Agencia
**			Campo3 = Ultimos 2 digitos do trace
**
*/


int criaChaveEvento(int Campo1, int Campo2, int Campo3)
{
	int key;

	if (Campo1 > 9000)
		Campo1 -= 8000;
	key = (Campo1 % 10000) * 1000000 + (Campo2 % 10000) * 100 + Campo3 % 100;
	logMsg("CriaChaveEvento: %u\n", key);
	return key;
}


int achaEvento(int Cpo1, int Cpo2, int Cpo3, struct cardco * cardcomsg)
{

	int keyvalue;
	int ret;

	keyvalue = criaChaveEvento(Cpo1, Cpo2, Cpo3);

	logMsg("AchaEvento: key = %d\n", keyvalue);


	obtemDadosDoEvento(keyvalue, cardcomsg);

	return ret;

}


struct cardco *localizaEvento(int key)
{
	struct listaEventos *lista;
	struct cardco *C_ptr;
	int pos;
	int found = 0;
	lista = gp;
	C_ptr = shmptr;

	pos = 0;


	for(pos = 0;  (lista->key != key) && (pos < MAXSLOTS); pos++) {
		if (lista->key == key) {

			/*	fprintf(fdbg,"Evento Localizada %s\n", NomeDaEvento); */
			found = 1;

		}
		else {
			C_ptr++;
			lista++;
		}
	}
	if(found) {
		return C_ptr;
	}
	/* Slot da Evento nao foi encontrado */
	return ( (struct cardco *) NULL);
}


int obtemDadosDoEvento(int key, struct cardco *C)
{
	struct cardco *C_ptr;
	C_ptr = localizaEvento(key);
	/* Altera o estado da central para bloqueada */
	if(C_ptr != (struct cardco *) NULL) {
		memcpy(C, C_ptr, sizeof(struct cardco));
		liberaEvento(key);
		return ((int) 1);
	}
	return ((int ) -1);
}

void liberaEvento(int key)
{
	struct listaEventos *lista;
	struct cardco *C_ptr;
	int pos;
	int found = 0;
	lista = gp;
	C_ptr = shmptr;

	pos = 0;


	for(pos = 0;  (lista->key != key) && (pos < MAXSLOTS); pos++) {
		if (lista->key == key) {
			lista->key = 0;
			lista->busy = 0;
			lista->timeStamp = 0;
			lista->lock = 0;

			memcpy(C_ptr, 0, sizeof(struct cardco));
		}
		else {
			C_ptr++;
			lista++;
		}
	}


}

struct cardco *obtemSlotLivre(int key)
{
	struct listaEventos *lista;
	struct cardco *C_ptr;
	int pos;
	int found = 0;
	lista = gp;
	C_ptr = shmptr;

	pos = 0;


	for(pos = 0;  (lista->busy != 0) && (pos < MAXSLOTS); pos++) {
		if (lista->busy == 0) {

			/*	fprintf(fdbg,"Evento Localizada %s\n", NomeDaEvento); */
			found = 1;

		}
		else {
			C_ptr++;
			lista++;
		}
	}
	if(found) {
		lista->key = key;
		lista->busy = 1;
		lista->timeStamp = stamp();
		lista->lock = 0;
		return C_ptr;
	}
	else {
		C_ptr = obtemSlotForced(key);
		return C_ptr;
	}
	/* Slot da Evento nao foi encontrado */
	return ( (struct cardco *) NULL);
}

struct cardco *obtemSlotForced(int key)
{
	struct listaEventos *lista;

	struct cardco *C_ptr;
	struct cardco * evento;
	struct listaEventos * slot;

	int pos;
	int found = 0;
	time_t tim;
	long difTime, newdif;
	lista = gp;
	C_ptr = shmptr;
	slot = gp;
	evento = shmptr;
	pos = 0;

	tim = stamp();
	difTime = 0;
	for(pos = 0;  pos < MAXSLOTS; pos++) {
		newdif = tim - lista->timeStamp;
		if (newdif > difTime) {

			difTime = newdif;
			evento = C_ptr;
			slot = lista;


		}
		C_ptr++;
		lista++;

	}
	slot->key = key;
	slot->busy = 1;
	slot->timeStamp = stamp();
	lista->lock = 0;
	return evento;

}


void gravaEvento(struct cardco *cardcomsg)
{
	struct cardco *C_ptr;
	int key;
	key = criaChaveEvento(cardcomsg->msg.trace, cardcomsg->msg.trantime, 0);
	C_ptr = obtemSlotLivre(key);
	memcpy(C_ptr, cardcomsg, sizeof(struct cardco));
}


time_t stamp ()
{
	time_t clock;
	struct tm  *tms;


	time(&clock);
	return clock;


}





















