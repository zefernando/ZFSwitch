/*#ident "@(#) fmt.c 1.00.0 13Mar05"*/
/*
 *	Formatador Visa
 *
 *	Entrada	Visa	ISO 8583
 *	Saida	Visa	ISO 8583
 *
 *      
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "cardco.h"
#include "extern.h"



void	fmtexit();
static void capturaTodosSinais(); 
extern int errno;
extern int criaStorage();
static char ver[]="Ext Interface: v1.0 25/02/2005";

main(int argc, char **argv)
{
	argv0 = *argv;
	capturaTodosSinais(fmtexit);
	
	
	/*
	 *	liga o debug
	 */
	fmt_debug = 	CARDCO_FMT_DEBUGIN | CARDCO_FMT_DEBUGOUT;
	debugOn("extern.debug");

	/*
	 *	Inicia o formatador
	 */
	fmt_init(FORMATTER_NAME);

	/*
	 *	Separa em formatador de entrada e saida
	 */
	pid = fork();
	if(pid < 0)
	{
		logMsg("%s:Nao foi possivel fazer o fork(): errno %d\n",__FILE__, errno);
		fmtexit(1);
	}


	if(pid == 0)
		extern_inbound();
	else
		extern_outbound();

	fmtexit(0);
}

static void capturaTodosSinais()
{
	int i;
	char auxb[50];
	for (i = 1 ; i < NSIG; i++ )
		if( signal(i,fmtexit) == SIG_ERR)  {
			// sprintf(auxb, "Erro no sinal %d\n", i);
			// perror(auxb);
		}

}

void fmtexit(int n)
{
	logMsg("%s:Saida devida ao sinal %d\n",__FILE__, n);

	if(pid > 0)
		/*	Envia o sinal para o processo filho */
		kill(pid, SIGTERM);
	else if(pid == 0)
		/*	Envia o sinal para o processo pai */
		kill(getppid(), SIGTERM);

	if(n > 1)
		kill(getpid(), n);
	exit(n);
}


fmt_init(char *name)
{
	/*
	 *	Inicia o formatador
	 */
	char	buf[128];
	logMsgSetPrefix("Visa");
    logMsg("%s Iniciado", name);


	extern800debug = 1;

	extern_support300 = 0;

	/* Cria os mapas de bits */
	create_bitmaps();
	criaFila();
	criaStorage();
}
