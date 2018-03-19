#include "fila.h" 
extern int errno;
extern void logMsg();


int criaFila()
{
	char path[80], *p;
	int msgid;
    	key_t chave; 
    	/*
	if((p = getenv("CARDCO_FILAS" )) == (char *) NULL) {
		printf("Variavel de ambiente CARDCO_FILAS nao esta definida\n");
		return (-1);
	}
	strcpy(path,p);
		
	strcat(path, "/filacardco");
	*/
	/* printf("Fila cardco %s\n", path); */
	// chave = ftok(path, 1);
	chave = 0x62617264;
	if(chave == -1) return -1;
/*        printf("Chave %x\n", chave); */
        if ((msgid = msgget(chave,  0660)) < 0) {
                printf("msgid nao localizou id\n");
	if ((msgid = msgget(chave,  IPC_CREAT | IPC_EXCL | 0660)) < 0) {

                       printf("msgget nao conseguiu criar id\n");

                }
        }
        return msgid;
}



int leFila(int tipo, unsigned char * buf, int len)
{
	int msgid;
	struct fila internalMsg;
	int ret = 0;
	if((msgid = criaFila() ) < 0)
	       	return(-1);
		
	ret = msgrcv(msgid,&internalMsg, len,tipo,0);
       	if( ret > 0) {
		memcpy(buf,internalMsg.buf, ret);
	}
	else {
		printf("leFila %s\n", strerror(errno));
	}
	return (ret);
}


int gravaFila(int tipo, char *buf, int len)
{
	int msgid;
	
	struct fila internalMsg;
	msgid = criaFila();
	internalMsg.tipo = tipo;
	memcpy(internalMsg.buf, buf,len); 
/*	printf("cardcomsg %s\n",cardcomsg.buf); */
	
	
	if(msgsnd(msgid, &internalMsg, len, IPC_NOWAIT) <0 )  {
		printf("GravaFila %s\n", strerror(errno));
		return -1;
	}		
	return (0);

}


