

/* ---- Header Files ----*/
#include <sys/types.h> 
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/file.h>
#include <ctype.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>


char Mensagem[2000]; /* String para mensagem de log */


/* ---- Prototipos ---- */
/* void	*signal(int sig, void (*func) (int));*/
void exit();
void reinicia(int sig);
void Inicia();
const char *TimeStamp();

void sigcatch( int sig);
int geraDbg(const char *msg);


extern  errno;

FILE *fdbg; /* Arquivo de debug */

struct processes {
	char path[80];
	char nome[80];
	char param1[80];
	char param2[80];
	int pid;
};

struct processes proc[] = { {"log","log", "cardco.log","", (int ) -1},
	                          {"paybal", "paybal","","", (int ) -1},
	                          {"fmt","fmt", "", "", (int ) -1},
	                          {"tcpclient", "tcpclient","", "", (int ) -1},
	                          {"auth", "auth","","", (int ) -1},
	                          {"", "", "", "", (int) -1}
                          };

/*
+---------------------------------------------------------------+ 
|Gera Formato de Data e Hora esperado para historicos e alertas |
+---------------------------------------------------------------+ 
*/
const char *TimeStamp()
{
	time_t clock;
	struct tm *tms;
	static char buf[32];

	time(&clock);
	tms = (struct tm *) localtime(&clock);
	if(tms->tm_year >= 100) tms->tm_year -= 100;

	sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d\0", tms->tm_year, tms->tm_mon + 1,
	        tms->tm_mday, tms->tm_hour,
	        tms->tm_min, tms->tm_sec);

	return buf;
}


/*
+-----------------------------------------------------------+
| Funcao IniciaP1 :                                         |
| Inicia processos P1 que gerenciam as portas 5000, recebe  |
| como parametro o numero da rede ao qual o processo inici- |
| ado ira gerenciar podendo receber como parametro o valor  |
| 1 ou 2.                                                   |
+-----------------------------------------------------------+
*/
void Inicia()
{
	/* Variaveis auxiliares */
	int  i ;
	int  pid_Aux;
	int index;

	sprintf(Mensagem,"%s Launch %d #OK#: Lancando processos ....\n",TimeStamp(),getpid());
	geraDbg(Mensagem);

	for( index = 0; index < 5 ; index++) {

		if ( (pid_Aux = fork()) == 0 ) {      /*   cria filho   */
			/* Ok e e' processo filho */
			execl(proc[index].path,proc[index].nome,proc[index].param1, proc[index].param2, (char *) NULL);
			sprintf(Mensagem,"%s Launch %d #OK#: processo %s nascendo ...\n",proc[index].nome, TimeStamp(),getpid());
			geraDbg(Mensagem);

		}

		if (pid_Aux < 0) { /* Erro */

			sprintf(Mensagem,"%s Launch %d #ERRO#: nao criou %s...\n",TimeStamp(),getpid(),proc[index].nome);
			geraDbg(Mensagem);

			sprintf(Mensagem,"%s Launch %d #ERRO#: vou me suicidar ....\n",TimeStamp(),getpid());
			geraDbg(Mensagem);
			kill(getpid(),SIGTERM);
			proc[index].pid = -1;
			return;
		}
		else{

			/* OK e e' pocesso pai */
			sprintf(Mensagem,"%s Launch %d #OK#: id do filho : %d \n",TimeStamp(),getpid(),pid_Aux);
			geraDbg(Mensagem);

			/* Atualiza variaveis globais com pid's dos processos filhos */
			proc[index].pid = pid_Aux;

			sprintf(Mensagem,"%s Launch %d #OK#: Atualizando Pid do %s para %d \n",TimeStamp(),getpid(),proc[index].nome, pid_Aux);
			geraDbg(Mensagem);
		}

	}


}

/*
+-----------------------------------------+
| Funcao chamada para termino do processo |
+-----------------------------------------+
*/
void reinicia(int sig)
{
	int   pid;
	int status;
	char  aux[2];
	char  cmd[PATH_MAX + 1];
	int found;
	int index;

	found = 0;




	if ((sig != SIGALRM) && (sig != SIGCLD)){

		sprintf(Mensagem,"%s Launch %d #AVISO#: Encerrando atividade\n",TimeStamp(),getpid());
		geraDbg(Mensagem);
		/*
			if (sig == SIGTERM ) {
		     		for( index = 0; index < 5 && (proc[index].nome != (char *) NULL); index++) {
					if(proc[index].pid != -1 ) {
		         			sprintf(Mensagem,"%s Launch %d #AVISO#:  %s Encerrando atividade\n", proc[index].nome, TimeStamp(),getpid());
					kill(proc[index].pid , SIGKILL);
					}
				} 
			}
		*/
		exit(0);
	}

	else {

		pid = wait(&status);
		index = 0;
		found = 0;
		sprintf(Mensagem,"%s Launch %d #AVISO#: processo %d morreu. Reiniciando processo ...\n",TimeStamp(),getpid(), pid);
		geraDbg(Mensagem);
		while( proc[index].nome[0] != '\0' ) {
			sprintf(Mensagem,"Procurando nome %s PID %d ...\n",proc[index].nome, proc[index].pid);
			geraDbg(Mensagem);
			if(index != 3 && pid == proc[index].pid) {
				found = 1;
				break;
			}
			index++;
		}


		if(found) {
			if ( (pid = fork()) == 0 ) {      /*   cria filho   */
				/* Ok e e' processo filho */
				execl(proc[index].path,proc[index].nome,proc[index].param1, proc[index].param2, (char *) NULL);
				sprintf(Mensagem,"Launch %s  %s Pid %d nascendo ...\n",proc[index].nome, TimeStamp(),getpid());
				geraDbg(Mensagem);
				printf(Mensagem);

			}

			if (pid < 0) { /* Erro */

				sprintf(Mensagem,"%s Launch %d #ERRO#: nao criou %s...\n",TimeStamp(),getpid(),proc[index].nome);
				geraDbg(Mensagem);

				sprintf(Mensagem,"%s Launch %d #ERRO#: vou me suicidar ....\n",TimeStamp(),getpid());
				geraDbg(Mensagem);
				kill(getpid(),SIGTERM);
				return;
			}
			else{

				/* OK e e' pocesso pai */
				sprintf(Mensagem,"%s Launch %d #OK#: id do filho : %d \n",TimeStamp(),getpid(),pid);
				geraDbg(Mensagem);

				/* Atualiza variaveis globais com pid's dos processos filhos */
				proc[index].pid = pid;

				sprintf(Mensagem,"%s Launch %d #OK#: Atualizando Pid do %s para %d \n",TimeStamp(),getpid(),proc[index].nome, pid);
				geraDbg(Mensagem);
			}
		}



	}


	// exit(0);
}


/*
+-------------------------------+
| Funcao para captura de sinais |
+-------------------------------+
*/
void    sigcatch (int sig)
{
	if (sig == SIGALRM) {

		sprintf(Mensagem,"%s Launch %d #AVISO#: capturou SIGALRM , reinicia\n",TimeStamp(),getpid());
		geraDbg(Mensagem);
	}
	else if (sig == SIGCLD) {

		sprintf(Mensagem,"%s Launch %d #AVISO#: capturou SIGCLD , reinicia\n",TimeStamp(),getpid());
		geraDbg(Mensagem);


	}
	else if (sig == SIGTERM) {

		sprintf(Mensagem,"%s Launch %d #AVISO#: capturou SIGTERM , termina !!!\n",TimeStamp(),getpid());
		geraDbg(Mensagem);


	}
	else {

		sprintf(Mensagem,"%s Launch %d #AVISO#: capturou sinal %d , termina !!!\n",TimeStamp(),getpid(),sig);
		geraDbg(Mensagem);


	}

	reinicia (sig);
}


int geraDbg(const char *msg)
{

	char   ArqDbg[PATH_MAX + 1];


	/* Pegando Variaveis de Ambiente */
	/* Na variavel FEB_DBG deve conter o diretorio de debug */
	sprintf(ArqDbg,"%s/launch.debug","./debug");

	/* Abrindo Arquivo de Debug */
	fdbg = fopen(ArqDbg,"a+");
	if (fdbg == NULL){
		printf("%s Launch %d #ERRO#: Falha na abertura do arquivo de debug: %s \n",TimeStamp(),getpid(),ArqDbg);
		return -1;
	}

	/* Grava mensagem */
	fprintf(fdbg,"%s\n",msg);
	fflush(fdbg);

	/* Fecha Arquivo */
	fclose(fdbg);

	return 0;

}


/*
 +----------------------------+
 |---- Programa Principal ----|
 +----------------------------+
*/
int main (int argc, char *argv[])
{

	int index;
	int    i;
	for( index = 0; index < 5 && strcmp(proc[index].nome,"tcpclient") != 0; index++) ;
	// IP
	strcpy(proc[index].param1, argv[1]);
	// PORTA
	strcpy(proc[index].param2, argv[2]);


	sprintf(Mensagem,"%s Launch %d #OK#: Iniciando programa de tratamento de mensagens. IP %s Porta %s\n",TimeStamp(),getpid(), proc[index].param1, proc[index].param2);
	printf(Mensagem);


	/* ---- Inicializa tratamento de sinais.----*/
	for (i = 1; i < NSIG; i++) signal(i,sigcatch);


	sprintf(Mensagem,"%s Launch %d #OK#: Processo Launch iniciado\n",TimeStamp(),getpid());
	geraDbg(Mensagem);

	Inicia();


	sprintf(Mensagem,"%s Launch %d #OK#: Processos criados, entrando em fase de monitoracao\n",TimeStamp(),getpid());
	geraDbg(Mensagem);


	while(1){

		/* printf("Launch %d : aguardando sinais\n",getpid());*/
		/* sleep(5); */
		/* Codigo acima alterado por codigo abaixo */

		sprintf(Mensagem,"%s Launch %d #OK#: Aguardando sinais\n",TimeStamp(),getpid());
		geraDbg(Mensagem);

		pause(); /* suspende o processamento ate' a ocorrencia de um sinal */

	}

}

