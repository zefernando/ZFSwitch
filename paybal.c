
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


#include "paybal.h"
#include "cardco.h"
// #include "fila.h"
#include <sqlca.h>
#include <sqlda.h>
#include <sqlcpr.h>
#define SQLCA_STORAGE_CLASS extern
#define ORACA_STORAGE_CLASS extern
#define FORMOUTQ	2
#define PAYBAL		7
struct	cardco	authmsg = {0,};
struct internalmsg *msg;
struct paybal  paybalArea, *paybalP;
extern int leFila(int tipo, unsigned char * buf, int len);
extern int gravaFila(int tipo, char *buf, int len);
extern char *__itoa (unsigned int value, char *string, unsigned int tam);
extern void logMsgSetPrefix(char * pfx);
extern void logMsg(const char * format, ...);
extern void debug(const char * format, ...);
extern void debugOn(const char * filename);
extern int printIMsg(struct internalmsg *msg);
void paybal(struct cardco * authmsg);
void paybalTerm(int term );
int initRequest();
char * getDateTime(char * tmp);
char * paybalRequest();
int doIt( struct internalmsg * authmsg , struct paybal *paybalP);
int prepareRequest();
int prepareResponse(char * resp);
char * authRequest(int route, char * paybalreq, char * paybalresp);
int connectDB(char * user, char * pwd, char * host, char * database, char *nome);
extern int callFcPagamento(char * req, char * resp, char * nome);
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
	initRequest();
	paybal(&authmsg);

}

void paybalTerm(int term)
{
	exit(term);
}

void paybal(struct cardco * authmsg)
{
	char * reqPtr;
	msg  = &authmsg->msg;
	paybalP = &paybalArea;
	int len;
	for(;;) {
		if(len = leFila(PAYBAL, (unsigned char *) msg , sizeof(struct internalmsg))== -1) {
			logMsg("Erro na Leitura da fila\n");
			continue;
		}
		else {
			//   printIMsg(msg);

			prepareRequest();
			doIt(msg, paybalP);

			// printIMsg(msg);
			// authTerm(1);
			if(gravaFila(FORMOUTQ, (char *) msg, sizeof(struct internalmsg)) < 0) {
				logMsg("Erro na gravacao da fila\n");
				paybalTerm(1);
				// logMsg("Erro na gravacao da fila\n");
			}

		}


	}
}

int initRequest()
{
	char user[80], pwd[80],host[80], conn[80];
	int i;
	logMsgSetPrefix("Auth");
	logMsg("Autorizador Iniciado");

	debugOn("paybal.debug");

	gNumCon = readIni();
	for(i = 0; i < gNumCon; i++) {
		if(connectDB(desc[i].user, desc[i].pwd,desc[i].host,desc[i].db, desc[i].conName) < 0) {
			logMsg("Erro de conexao Oracle");
			paybalTerm(1);
		}

		logMsg("Conexao com Oracle Ok.");
	}
}

/* Le o arquivo descritivo das conexoes */
int readIni()
{
	FILE * ini;
	char fileName[256];
	int i;
	int numCon;

	strcpy(fileName, "paybal.ini");

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

	numCon = --i;
	fclose(ini);

	return numCon;
}

int prepareRequest()
{
	char tmp[80] = {0,};
	char auxbuf[512] = {0,};
	char *ptr;
	int len =0;
	int result = SUCCESS;
	debug("Prepare Request\n");
	memset(paybalP, 0x00, sizeof(paybalArea));
	__itoa(msg->msgtype,paybalP->msgType,4);
	memcpy(auxbuf,paybalP->msgType,4);
	auxbuf[4] = '\0';
	debug("MsgType %s\n", auxbuf);
	__itoa(msg->pcode,paybalP->pcode,6);
	memcpy(auxbuf,paybalP->pcode,6);
	auxbuf[6] = '\0';
	debug("pcode %s\n", auxbuf);
	memcpy(paybalP->nsu,msg->refnum,CARDCO_REFNUM_LEN);
	memcpy(auxbuf,paybalP->nsu, CARDCO_REFNUM_LEN);
	auxbuf[CARDCO_REFNUM_LEN] = '\0';
	debug("nsu %s\n", auxbuf);
	paybalP->origem = 'P';

	memset(paybalP->merchant, '0',4);
	memcpy(&paybalP->merchant[4],&msg->termloc[7],8);
	memcpy(auxbuf,paybalP->merchant,12);
	auxbuf[12] = '\0';
	debug("merchant %s\n", auxbuf);
	memcpy(paybalP->termid,msg->termid,CARDCO_TERMID_LEN);
	memcpy(auxbuf,paybalP->termid,CARDCO_TERMID_LEN);
	auxbuf[CARDCO_TERMID_LEN] = '\0';
	debug("Termid %s\n", auxbuf);
	__itoa((int) msg->merchant_type, paybalP->mcc,4);
	memcpy(auxbuf,paybalP->mcc,4);
	auxbuf[4] = '\0';
	debug("mcc %s\n", auxbuf);

	len = strlen(msg->pan);
	ptr = paybalP->pan;
	if(len < 19) {
		memset(ptr ,'0', 19 -len);
		ptr += (19 - len);
	}
	memcpy(ptr,msg->pan,19);

	memcpy(auxbuf,paybalP->pan,19);
	auxbuf[19] = '\0';
	// printf("pan %s\n", auxbuf);
	if(strlen((char *) msg->track2 + 1) == 0 ) {
		memset(paybalP->expdate,'0',4);
	}	else {
	        memcpy(paybalP->expdate, (char *) msg->track2 + 1,4);
	}
	memcpy(auxbuf,paybalP->expdate,4);
	auxbuf[4] = '\0';
	debug("expdate %s\n", auxbuf);
	memcpy(paybalP->amount, msg->amount,12);
	memcpy(auxbuf,paybalP->amount,12);
	auxbuf[12] = '\0';
	debug("amount %s\n", auxbuf);

	paybalP->txnType = msg->acceptorname[4];
	debug("Tipo transacao %c \n", paybalP->txnType);
	if(strlen((char *) (msg->acceptorname + 5)) == 0 ) {
		memset(paybalP->dtaPagto,'0',8);
	} else
		memcpy(paybalP->dtaPagto, (char *) (msg->acceptorname + 5), 8);

	memcpy(auxbuf,paybalP->dtaPagto,8);
	auxbuf[8] = '\0';
	debug("Data Pagamento %s\n", auxbuf);
	if(strlen((char *) (msg->acceptorname + 13)) == 0 ) {
		memset(paybalP->banco,'0',4);
	} else
		memcpy(paybalP->banco, (char *) (msg->acceptorname + 13), 4);
	memcpy(auxbuf,paybalP->banco,4);
	auxbuf[4] = '\0';
	debug("banco %s\n", auxbuf);
	if(strlen((char *) (msg->acceptorname + 17)) == 0 ) {
		memset(paybalP->agencia,'0',4);
	} else
		memcpy(paybalP->agencia, (char *) (msg->acceptorname + 17), 4);
	memcpy(auxbuf,paybalP->agencia,4);
	auxbuf[4] = '\0';
	debug("agencia %s\n", auxbuf);
	if(strlen((char *) (msg->acceptorname + 21)) == 0 ) {
		memset(paybalP->conta,'0',10);
	} else
		memcpy(paybalP->conta, (char *) (msg->acceptorname + 21), 10);
	memcpy(auxbuf,paybalP->conta,10);
	auxbuf[10] = '\0';
	debug("conta %s\n", auxbuf);
	if(strlen((char *) (msg->acceptorname + 31)) == 0 ) {
		memset(paybalP->numCheque,'0',6);
	} else
		memcpy(paybalP->numCheque, (char *) (msg->acceptorname + 31), 6);
	memcpy(auxbuf,paybalP->numCheque,6);
	auxbuf[6] = '\0';
	debug("Numero do cheque %s\n", auxbuf);

	memset(paybalP->respcode,'0',2);
	memset(paybalP->erro,'0', 4);
	memset(paybalP->authcode,'0', 6);

	memset(paybalP->creditoDisp,'0', 12);
	memset(paybalP->saldoDev,'0', 12);

	memset(paybalP->valorMinimo,'0', 12);
	memset(paybalP->vencimento,'0', 6);
	memset(paybalP->valorTarifa,'0', 6);

	memset(paybalP->dtaHora,'0', 12);

	memset(auxbuf, 0, sizeof(struct paybal) + 1);
	memcpy(auxbuf,(char *) paybalP, sizeof(struct paybal));

	debug("Area Auth %s \n", auxbuf );
	// fflush(stdout);

}

int prepareResponse(char * resp)
{
	char respcode[3];
	char date[9];
	debug("Prepare Response");
	struct paybal *paybalP;
	debug("Area Resp %s\n", resp);
	paybalP = (struct paybal *) resp;
	memcpy(msg->authnum, paybalP->authcode,6);
	msg->authnum[6] = '\0';
	debug("authnum %s\n", msg->authnum);
	memcpy(respcode, paybalP->respcode,2);
	respcode[2] = '\0';
	debug("Respcode %s\n", respcode);
	msg->respcode = atoi(respcode);
	msg->msgtype += 10;
	// Credito disponivel
	memcpy(msg->aval_balance, paybalP->creditoDisp,12);
	msg->aval_balance[12] = '\0';
	debug("Credito disponivel(aval_balance) %s\n", msg->aval_balance);
	// Saldo devedor
	memcpy(msg->ledger_balance, paybalP->saldoDev,12);
	msg->ledger_balance[12] = '\0';
	debug("Saldo devedor(ledger balance) %s\n", msg->ledger_balance);
	// Valor minimo de pagamento
	memcpy(msg->settlement_amount,paybalP->valorMinimo,12);
	msg->settlement_amount[12] = '\0';
	debug("Valor minimo(settlement) %s\n", msg->settlement_amount);
	memcpy(date, paybalP->vencimento,6);
	date[6] = '\0';
	// Data de vencimento
	debug("Data de Vencimento %s\n", date);
	msg->settlement_date = atol(date);
	debug("Data de Vencimento %06ld\n", msg->settlement_date);
	// Tarifa
	memcpy(msg->fee, paybalP->valorTarifa,6);
	msg->fee[6] = '\0';
	debug("Tarifa %s\n", msg->fee);
	// Data da transacao
	memcpy(msg->dth_upd, paybalP->dtaHora,12);
	msg->dth_upd[12] = '\0';
	debug("Data de atualizacao %s\n", msg->dth_upd);
	debug("Response prepared");

}

int doIt( struct internalmsg * authmsg, struct paybal * paybalP  )
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
		logMsg("\n   **** Consulta/Pagamento ***** \n");
		route = getRoute(bin);

		break;
	case CARDCO_REVREQ:
	case CARDCO_REVREQ_REPEAT:
	case CARDCO_REVREQ_ADVICE:
		logMsg("\n   **** Reversao ***** \n");
		if (strncmp("003", authmsg->termid,3 ) == 0){
			route = 2;
		}
		else
			if (strncmp("013", authmsg->termid,3 ) == 0){
				route = 1;
			}
			else
				if (strncmp("023", authmsg->termid,3 ) == 0){
					route = 3;
				}
		break;
	}

	if(route >= 0 && paybalRequest(route, (char *) paybalP,resp) != (char *) NULL ) {
		prepareResponse(resp);
	}
	else  {
		if(route < 0)
			logMsg("Bin desconhecido\n");
		msg->msgtype += 10;
		msg->respcode = 96;
	}


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

char * paybalRequest(int route, char * paybalreq, char * paybalresp)
{
	char * respPtr;
	char conexao[80];
	int sqlcode = 0;
	debug("paybalRequest\n");

	sqlcode = callFcPagamento(paybalreq, paybalresp, desc[route].conName);
	if( sqlcode != 0 ) {
		logMsg( "sqlcode.code	 : %d\n"   , sqlcode );
		// printf( "sqlcode.call    : <%s>\n" , sqlca.sqlerrm.sqlerrmc );
		respPtr = (char *) NULL;
	}
	else  {
		logMsg( "Autorizacao Nova %s", conexao);
		respPtr = paybalresp;
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
	//	 printf("Conexao %s\n", conexao);
	if( database != NULL )
		sprintf( username , "%s@%s" , user , database );
	strcpy(server,host);
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
