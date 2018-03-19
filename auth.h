#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
		
#define JARDIMSUL	"627776"
#define ALFA		"627576"
#define CARDCO		"636182"
#define CONNJDSUL	"connJardimsul"
#define CONNALFA	"connAlfa"
#define CONNCARD	"connCard"
#define HOST		"TESTE"
#define USER = "admc@orcl"
#define PWD = "admc"

#define JDSULMSG	"\n   **** Cartao JardimSul ***** \n"
#define REDEMAISMSG	"\n   **** Cartao RedeMais ***** \n"
#define CARDCOMSG	"\n   **** Cartao Adm CardCo ***** \n"
#define SUCCESS 1



struct auth {
	char msgType[4]; 
	char pcode[6];			// 3
	char nsu[12];			// 37
	char origem;			// P
	char merchant[12];		// 42
	char termid[8]; 		// 41
	char mcc[4];			// 18
	char pan[19];			// 02
	char expdate[4];		// 14
	char amount[12];		// 04
	char entrada[12];		//
	char pin[8];			// 52
	char tipoParc;			// 43-6
	char qtdParc[2];		// 43-7->8
	char plano[3];			// 43-20->24
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
};
