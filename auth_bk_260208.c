

/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
struct sql_cursor
{
	unsigned int curocn;
	void *ptr1;
	void *ptr2;
	unsigned long magic;
};
typedef struct sql_cursor sql_cursor;
typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
	unsigned char *tvnvsn;
	unsigned short tvnvsnl;
	unsigned char *tvnnm;
	unsigned short tvnnml;
	unsigned char *tvnsnm;
	unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
	unsigned int adtvsn;
	unsigned short adtmode;
	unsigned short adtnum;
	sqltvn adttvn[1];
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
				       1,1,0,
			       };

/* Binding to PL/SQL Records */
struct sqltdss
{
	unsigned int tdsvsn;
	unsigned short tdsnum;
	unsigned char *tdsval[1];
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
	{
		1,
		0,
	};

/* File name & Package Name */
struct sqlcxp
{
	unsigned short fillen;
	char  filnam[8];
};
static const struct sqlcxp sqlfpn =
	{
		7,
		"auth.pc"
	};


static unsigned long sqlctx = 9211;


static struct sqlexd {
	unsigned int   sqlvsn;
	unsigned int   arrsiz;
	unsigned int   iters;
	unsigned int   offset;
	unsigned short selerr;
	unsigned short sqlety;
	unsigned int   occurs;
	const short *cud;
	unsigned char  *sqlest;
	const char  *stmt;
	sqladts *sqladtp;
	sqltdss *sqltdsp;
	void  **sqphsv;
	unsigned int   *sqphsl;
	int   *sqphss;
	void  **sqpind;
	int   *sqpins;
	unsigned int   *sqparm;
	unsigned int   **sqparc;
	unsigned short	*sqpadto;
	unsigned short	*sqptdso;
	void  *sqhstv[5];
	unsigned int   sqhstl[5];
	int   sqhsts[5];
	void  *sqindv[5];
	int   sqinds[5];
	unsigned int   sqharm[5];
	unsigned int   *sqharc[5];
	unsigned short	sqadto[5];
	unsigned short	sqtdso[5];
} sqlstm = {10,5};

// Prototypes
extern	void sqlcxt (void **, unsigned long *,
			     struct sqlexd *, const struct sqlcxp *);
extern	void sqlcx2t(void **, unsigned long *,
			     struct sqlexd *, const struct sqlcxp *);
extern	void sqlbuft(void **, char *);
extern	void sqlgs2t(void **, char *);
extern	void sqlorat(void **, unsigned long *, void *);

// Forms Interface
static const int IAPSUCC = 0;
static const int IAPFAIL = 1403;
static const int IAPFTL  = 535;
extern	void sqliem(char *, int *);

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
	{10,4130,0,0,0,
	 5,0,0,1,34,0,1146,210,0,0,2,2,0,0,128,1,97,0,0,1,97,0,0,1,97,0,0,
	 32,0,0,2,34,0,1146,218,0,0,2,2,0,0,128,1,97,0,0,1,97,0,0,1,97,0,0,
	 59,0,0,3,0,0,1051,287,0,0,4,4,0,0,128,1,97,0,0,1,97,0,0,1,10,0,0,1,10,0,0,1,97,
	 0,0,
	 94,0,0,4,0,0,32,305,0,0,0,0,0,1,0,
	};


#include "auth.h"
#include "cardco.h"
// #include "fila.h"
#include <sqlca.h>
#include <sqlda.h>
#include <sqlcpr.h>
#define SQLCA_STORAGE_CLASS extern
#define ORACA_STORAGE_CLASS extern
#define FORMOUTQ	2
#define AUTHQ		3
struct	cardco	authmsg = {0,};
struct internalmsg *msg;
struct auth  authArea, *authP;
extern int leFila(int tipo, unsigned char * buf, int len);
extern int gravaFila(int tipo, char *buf, int len);
extern char *__itoa (unsigned int value, char *string, unsigned int tam);
extern void logMsgSetPrefix(char * pfx);
extern void logMsg(const char * format, ...);
extern void debug(const char * format, ...);
extern void debugOn(const char * filename);
extern int printIMsg(struct internalmsg *msg);
void auth(struct cardco * authmsg);
void authTerm(int term );
int initAuth();
char * getDateTime(char * tmp);
char * authRequest();
int authorize( struct internalmsg * authmsg , struct auth *authP);
int prepareAuthorization();
int prepareResponse(char * resp);
char * authRequest(int route, char * authreq, char * authresp);
int connectDB(char * user, char * pwd, char * host, char * database, char *nome);
extern int callFcAutorizacao(char * req, char * resp, char * nome);
void stripTraillingBlanks(char * str);
struct connection {
	char user[20];
	char pwd[20];
	char host[20];
	char db[20];
	char conName[20];
	char bin[10];
};
typedef struct connection con_t;

con_t desc[100];
int gNumCon = 0;

/* exec sql begin declare section; */

char request[512], response[512],connAlfa[80], connJdSul[80], connCard[80];
/* exec sql end   declare section; */


main(int argc, char *argv[])
{
	initAuth();
	auth(&authmsg);

}

void authTerm(int term)
{
	exit(term);
}

void auth(struct cardco * authmsg)
{
	char * reqPtr;
	msg  = &authmsg->msg;
	authP = &authArea;
	int len;
	for(;;) {
		if(len = leFila(AUTHQ, (unsigned char *) msg , sizeof(struct internalmsg))== -1) {
			logMsg("Erro na Leitura da fila\n");
			continue;
		}
		else {
			//   printIMsg(msg);

			prepareAuthorization();
			authorize(msg, authP);

			// printIMsg(msg);
			// authTerm(1);
			if(gravaFila(FORMOUTQ, (char *) msg, sizeof(struct internalmsg)) < 0) {
				logMsg("Erro na gravacao da fila\n");
				authTerm(1);
				// logMsg("Erro na gravacao da fila\n");
			}

		}


	}
}

int initAuth()
{
	char user[80], pwd[80],host[80], conn[80];
	int i;

	logMsgSetPrefix("Auth");
	logMsg("Autorizador Iniciado");
	// sql_connect("jardimsul", "jardimsul", "icreddb1");	/* Teste */
	//  sql_connect("alfa2", "alfa2", "icreddb1");	 /* Teste */

	debugOn("auth.debug");
	gNumCon = readIni();
	for(i = 0; i < gNumCon; i++) {
			debug("U:%s\tP:%s\tH:%s\tD:%s\tC:%s\tB:%s\n",desc[i].user, desc[i].pwd, desc[i].host, desc[i].db,
		      desc[i].conName, desc[i].bin);
		if(connectDB(desc[i].user, desc[i].pwd,desc[i].host,desc[i].db, desc[i].conName) < 0) {
			logMsg("Erro de conexao Oracle");
			authTerm(1);
		}

		logMsg("Conexao com Oracle Ok.");
	}
	
	
}

int prepareAuthorization()
{
	char tmp[80] = {0,};
	char auxbuf[512] = {0,};
	char *ptr;
	int len =0;
	int result = SUCCESS;
	debug("Prepare Authorization\n");
	memset(authP, 0x00, sizeof(authArea));
	__itoa(msg->msgtype,authP->msgType,4);
	memcpy(auxbuf,authP->msgType,4);
	auxbuf[4] = '\0';
	debug("MsgType %s\n", auxbuf);
	__itoa(msg->pcode,authP->pcode,6);
	memcpy(auxbuf,authP->pcode,6);
	auxbuf[6] = '\0';
	debug("pcode %s\n", auxbuf);
	memcpy(authP->nsu,msg->refnum,CARDCO_REFNUM_LEN);
	memcpy(auxbuf,authP->nsu, CARDCO_REFNUM_LEN);
	auxbuf[CARDCO_REFNUM_LEN] = '\0';
	debug("nsu %s\n", auxbuf);
	authP->origem = 'P';
	memset(authP->merchant, '0',4);
	memcpy(&authP->merchant[4],&msg->termloc[7],8);
	memcpy(auxbuf,authP->merchant,12);
	auxbuf[12] = '\0';
	debug("merchant %s\n", auxbuf);
	memcpy(authP->termid,msg->termid,CARDCO_TERMID_LEN);
	memcpy(auxbuf,authP->termid,CARDCO_TERMID_LEN);
	auxbuf[CARDCO_TERMID_LEN] = '\0';
	debug("Termid %s\n", auxbuf);
	__itoa((int) msg->merchant_type, authP->mcc,4);
	memcpy(auxbuf,authP->mcc,4);
	auxbuf[4] = '\0';
	debug("mcc %s\n", auxbuf);

	len = strlen(msg->pan);
	ptr = authP->pan;
	if(len < 19) {
		memset(ptr ,'0', 19 -len);
		ptr += (19 - len);
	}
	memcpy(ptr,msg->pan,19);

	memcpy(auxbuf,authP->pan,19);
	auxbuf[19] = '\0';
	// printf("pan %s\n", auxbuf);
	if(strlen((char *) msg->track2 + 1) == 0 ) {
		memset(authP->expdate,'0',4);
	}	else {
		memcpy(authP->expdate, (char *) msg->track2 + 1,4);
	}
	memcpy(auxbuf,authP->expdate,4);
	auxbuf[4] = '\0';
	debug("expdate %s\n", auxbuf);
	memcpy(authP->amount, msg->amount,12);
	memcpy(auxbuf,authP->amount,12);
	auxbuf[12] = '\0';
	debug("amount %s\n", auxbuf);
	debug("pin len %d\n", strlen(msg->pin));
	memset(authP->entrada, '0', 12);
	if(strlen(msg->pin) == 0) {
		memset(authP->pin, '0', 8);
	}
	else {
		memcpy(authP->pin,msg->pin,8);
	}
	authP->tipoParc = msg->acceptorname[5];
	debug("Tipo parc %c \n", authP->tipoParc);
	if(strlen((char *) (msg->acceptorname + 6)) == 0 ) {
		memset(authP->qtdParc,'0',2);
	} else 
		memcpy(authP->qtdParc, (char *) (msg->acceptorname + 6), 2);

	memcpy(auxbuf,authP->qtdParc,2);
	auxbuf[2] = '\0';
	debug("qtdPArc %s\n", auxbuf);
	if(strlen((char *) (msg->acceptorname + 19)) == 0 ) {
		memset(authP->plano,'0',3);
	} else 
		memcpy(authP->plano, (char *) (msg->acceptorname + 19), 3);
	memcpy(auxbuf,authP->plano,3);
	auxbuf[3] = '\0';
	debug("plano %s\n", auxbuf);

	/*
	 *
	 char respcode[2];
		 char erro[4];
		 char authcode[6];
		 char vencimento[8];
		 char valorParc[12];
		 char valorFinanc[12];
		 char taxaJuros[4];
		 char dispcompras[12];
		 char senhaLetras[3];
		 char senhaRandom[3];
	 *
	*/ 
	memset(authP->respcode,'0',2);
	memset(authP->erro,'0', 4);
	memset(authP->authcode,'0', 6);
	memset(authP->vencimento,'0', 8);
	memset(authP->valorParc,'0', 12);
	memset(authP->valorFinanc,'0', 12);
	memset(authP->taxaJuros,'0', 4);

	memset(authP->dispcompras,'0', 12);
	memset(authP->senhaLetras,'0', 3);
	memset(authP->senhaRandom,'0', 3);
	memset(auxbuf, 0, sizeof(struct auth) + 1);
	memcpy(auxbuf,(char *) authP, sizeof(struct auth));

	debug("Area Auth %s \n", auxbuf );
	// fflush(stdout);

}

int prepareResponse(char * resp)
{
	char respcode[3];

	debug("Prepare Response");
	struct auth *authP;
	authP = (struct auth *) resp;
	memcpy(msg->authnum, authP->authcode,6);
	msg->authnum[6] = '\0';
	debug("authnum %s\n", msg->authnum);
	memcpy(respcode, authP->respcode,2);
        
    respcode[2] = '\0';
	if(atoi(msg->authnum) == 0 && atoi(respcode) == 0) {
		memcpy(respcode,"91",2);
	}
	debug("Respcode %s\n", respcode);
	msg->respcode = atoi(respcode);
	msg->msgtype += 10;
    debug("Pos entry code %d\n", msg->pos_entry_code);
    
    debug("authP %s\n", authP);    
    
	memcpy(msg->amount, authP->valorFinanc,12);
	debug("msg amount %s\n", msg->amount);
    
   
	debug("Response prepared");

}

int authorize( struct internalmsg * authmsg, struct auth * authP  )
{

	char bin[9];
	char termid[9];
	int route = 0;
	char * reqPtr;
	char resp[512];
	char auxResp[512];
	debug("Authorize\n");
	memset(resp   , 0 , sizeof(resp) );
	strncpy(bin, authmsg->pan, 6);
	bin[6] = '\0';
	debug("bin %s\n", bin);
	switch(authmsg->msgtype){
	case CARDCO_AUTHREQ:
	case CARDCO_AUTHREQ_REPEAT:
	case CARDCO_AUTHREQ_ADVICE:
	case CARDCO_FINREQ:
	case CARDCO_FINREQ_REPEAT:
	case CARDCO_FINREQ_ADVICE:
		logMsg("\n   **** Venda ***** \n");
		route = getRoute(bin);
		break;
	case CARDCO_REVREQ:
	case CARDCO_REVREQ_REPEAT:
	case CARDCO_REVREQ_ADVICE:
		logMsg("\n   **** Reversao ***** \n");
		route = getRoute(bin);
		if(route == -1) {
			if (strncmp("003", authmsg->termid,3 ) == 0){
				route = 0;
			}
			else
			if (strncmp("013", authmsg->termid,3 ) == 0){
				route = 1;
			}
			else
			if (strncmp("023", authmsg->termid,3 ) == 0){
				route = 1;
			}
		}
		break;
	}
	debug("Route : %d \n", route);
	if(authRequest(route, (char *) authP,resp) != (char *) NULL ) {
		prepareResponse(resp);
	}
	else  {
		msg->msgtype += 10;
		msg->respcode = 96;
	}


}

/* Le o arquivo descritivo das conexoes */
int readIni()
{
	FILE * ini;
	char fileName[256];
	int i;
	int numCon;

	strcpy(fileName, "auth.ini");

	debug("\nNome do arquivo ini%s\n", fileName);
	ini = fopen(fileName, "r");
	if(ini == (FILE *) NULL) {
		debug("Nao abriu arquivo ini: %s\n", fileName);
		exit(0);
	}
	numCon = 0;
	i = 0;

	while(fscanf(ini,"%s%s%s%s%s%s",desc[i].user, desc[i].pwd, desc[i].host,desc[i].db,desc[i].conName, desc[i].bin) != EOF)
	{
		debug("%s\t%s\t%s\t%s\t%s\t%s\n",desc[i].user, desc[i].pwd, desc[i].host, desc[i].db,
		      desc[i].conName, desc[i].bin);
		i++;
	}

	numCon = i;
	fclose(ini);

	return numCon;
}

int getRoute(char *bin)
{
	int i = 0;
	int route = -1;
	for(i = 0; i < gNumCon; i++) {
		if (strncmp(desc[i].bin, bin, 6) == 0){
			route = i;
			break;
		}
	}
	return route;
}

char * authRequest(int route, char * authreq, char * authresp)
{
	char * respPtr;
	char conexao[80];
	int sqlcode = 0;
	debug("authRequest %s\n", desc[route].conName);

	sqlcode = callFcAutorizacao(authreq, authresp, desc[route].conName);
	if( sqlcode != 0 ) {
		logMsg( "sqlcode.code	 : %d\n"   , sqlcode );
		respPtr = (char *) NULL;
	}
	else  {
		logMsg( "Conexao Ativa Autorizacao Nova %s", conexao);
		// if(checkResponseSize(response)) {
		stripTraillingBlanks(authresp);
		respPtr = authresp;
				
	}
	return(respPtr);
}

int checkResponseSize(char * resp)
{
	int ret;
	int len;
	char respSiz[4];
	ret = 1;
	debug("checkResponseSize");
	len = strlen(response);

	strncpy( respSiz , response + 1 , 3 );
	respSiz[3] = '\0';

	if( atoi(respSiz) != (len - 4) ) {
		logMsg( "sql_autorizacao->Wrong Size(%s !!!\n" , respSiz );
		ret = 0;
	}
	return ret;
}

void stripTraillingBlanks(char * str)
{
	char * ptr;
	int len, count;
	len = strlen(str);
	ptr = str + len -1;
	for (count = len -1 ; count >= 0 && *ptr == 0x20; count--) *ptr-- = '\0';
}

int connectDB(char * user, char * pwd, char * host, char * database, char *nome)
{
	/* exec sql begin declare section; */

	char username[80], password[80], server[80], conexao[80];
	/* exec sql end   declare section; */

	char buffer[100];
	strcpy(username, user);
	strcpy(password, pwd);
	strcpy(conexao, nome);
		 debug("Conexao %s\n", conexao);
		 debug("pwd %s\n", password);
	if( database != NULL )
		sprintf( username , "%s@%s" , user , database );
	strcpy(server,host);
		 debug("user %s\n", username);
	// exec sql connect :username IDENTIFIED BY :password AT :conexao USING :server;
	/* exec sql connect :username IDENTIFIED BY :password AT :conexao; */

	{
		struct sqlexd sqlstm;
		sqlstm.sqlvsn = 10;
		sqlstm.arrsiz = 5;
		sqlstm.sqladtp = &sqladt;
		sqlstm.sqltdsp = &sqltds;
		sqlstm.iters = (unsigned int  )10;
		sqlstm.offset = (unsigned int  )59;
		sqlstm.cud = sqlcud0;
		sqlstm.sqlest = (unsigned char	*)&sqlca;
		sqlstm.sqlety = (unsigned short)256;
		sqlstm.occurs = (unsigned int  )0;
		sqlstm.sqhstv[0] = (	     void  *)username;
		sqlstm.sqhstl[0] = (unsigned int  )80;
		sqlstm.sqhsts[0] = (	     int  )80;
		sqlstm.sqindv[0] = (	     void  *)0;
		sqlstm.sqinds[0] = (	     int  )0;
		sqlstm.sqharm[0] = (unsigned int  )0;
		sqlstm.sqadto[0] = (unsigned short )0;
		sqlstm.sqtdso[0] = (unsigned short )0;
		sqlstm.sqhstv[1] = (	     void  *)password;
		sqlstm.sqhstl[1] = (unsigned int  )80;
		sqlstm.sqhsts[1] = (	     int  )80;
		sqlstm.sqindv[1] = (	     void  *)0;
		sqlstm.sqinds[1] = (	     int  )0;
		sqlstm.sqharm[1] = (unsigned int  )0;
		sqlstm.sqadto[1] = (unsigned short )0;
		sqlstm.sqtdso[1] = (unsigned short )0;
		sqlstm.sqhstv[4] = (	     void  *)conexao;
		sqlstm.sqhstl[4] = (unsigned int  )80;
		sqlstm.sqhsts[4] = (	     int  )80;
		sqlstm.sqindv[4] = (	     void  *)0;
		sqlstm.sqinds[4] = (	     int  )0;
		sqlstm.sqharm[4] = (unsigned int  )0;
		sqlstm.sqadto[4] = (unsigned short )0;
		sqlstm.sqtdso[4] = (unsigned short )0;
		sqlstm.sqphsv = sqlstm.sqhstv;
		sqlstm.sqphsl = sqlstm.sqhstl;
		sqlstm.sqphss = sqlstm.sqhsts;
		sqlstm.sqpind = sqlstm.sqindv;
		sqlstm.sqpins = sqlstm.sqinds;
		sqlstm.sqparm = sqlstm.sqharm;
		sqlstm.sqparc = sqlstm.sqharc;
		sqlstm.sqpadto = sqlstm.sqadto;
		sqlstm.sqptdso = sqlstm.sqtdso;
		sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
	}


	logMsg( "sqlcode.connect : <%s>\n" , sqlca.sqlerrm.sqlerrmc );
	if( sqlca.sqlcode != 0 ) {
		strcpy( buffer , sqlca.sqlerrm.sqlerrmc );
		logMsg( "sqlcode.call	 : <%s>\n" , buffer );
		return -1;
	}
	logMsg( "sqlcode.code	 : %d\n"   , sqlca.sqlcode );
	logMsg( "Conexao nomeada estabelecida - 02.\n");
	return 1;
}


void sql_error()
{
	/* exec SQL WHENEVER SQLERROR CONTINUE; */

	logMsg("\nOracle error detected:\n");
	logMsg("\n% .70s \n", sqlca.sqlerrm.sqlerrmc);
	/* exec SQL ROLLBACK WORK RELEASE; */

	{
		struct sqlexd sqlstm;
		sqlstm.sqlvsn = 10;
		sqlstm.arrsiz = 5;
		sqlstm.sqladtp = &sqladt;
		sqlstm.sqltdsp = &sqltds;
		sqlstm.iters = (unsigned int  )1;
		sqlstm.offset = (unsigned int  )94;
		sqlstm.cud = sqlcud0;
		sqlstm.sqlest = (unsigned char	*)&sqlca;
		sqlstm.sqlety = (unsigned short)256;
		sqlstm.occurs = (unsigned int  )0;
		sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
	}


	exit(1);
}
