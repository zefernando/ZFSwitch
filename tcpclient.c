#include "sockhelp.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <fila.h>

void run(char *, int);
void *ToNet();
void *FromNet();


extern int errno;
int sock = -1;
FILE *logfd;	


int main(int argc, char *argv[])
{

//	ignore_pipe();
printf( "%s %d", argv[1], atoi(argv[2]));
fflush(stdout);
	run(argv[1], atoi(argv[2]));
	return 0;
}

void run(char *ip, int port)
{
	int rc;
	char porta[6];
	int * portPtr;
	
	int connected = 1;
	pthread_t fromThread;
	pthread_t toThread;

//	portPtr = (int *) port;
	logMsgSetPrefix("TCP/IP Driver");
	sprintf(porta,"%d", port);
	logMsg("Porta %s\n", porta);

	sock = make_connection(porta, ip);
	if (sock == -1) {
		 logMsg("make_connection failed.\n");
		 fprintf(stderr,"Conexao falhou, Erro: %s.\n", strerror(errno));

	}
	else {
	
		
		rc = pthread_create(&fromThread, NULL, FromNet, (void *) &sock);
		if (rc){
			logMsg("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}

		rc = pthread_create(&toThread, NULL, ToNet, (void *) &sock);
		if (rc){
			logMsg("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		while (1) {
			sleep(2);
		}
//		close(sock);
	}
	termina();
}

void *FromNet(void * socket) {
	int sock;
	int *sockPtr;
	int len;
	char buffer[1024];

	sockPtr = (int *) socket;
	sock = * sockPtr;
		
	logfd = (FILE *) logOn(logfd, "logtcp.log");

	while(1) {
		len = rcvFromNet(sock, buffer);
		if(len > 0) {
			logTcp(logfd, buffer,len,"From Visa"); 
			gravaFila(FORMINQ, buffer, len);
		}
		else {
			termina();
		}

	}
	
}

void *ToNet(void * socket) {
	int sock;
	int len;
	int *sockPtr;
	char buffer[1024];

	sockPtr = (int *) socket;
	sock = * sockPtr;
	
	while(1) {
		len = leFila(DRIVERQ,buffer, sizeof(buffer));
		if(len > 0) {
			if(sendToNet(sock, buffer, len) < 0) termina();
		}
		else {
			termina();
		}
	}

}
termina()
{
	if(sock > 0) close(sock);
	if(logfd != (FILE *) NULL ) fclose(logfd);
	sleep(1);
	system("./onTcpError.sh");
	exit(0);
}



