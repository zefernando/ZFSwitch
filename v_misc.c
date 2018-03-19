#include <stdio.h>
#include <ctype.h>
#include "cardco.h"
#include <stdarg.h>
#include <time.h>
#include "fila.h"
#include "extern.h"

extern char prefix[];

void toZoned( char *, char *, int);
int toBinary( char *, char *, int);
short btos(unsigned char * src, int len);
int itob(char *dst, int src, int len);
void debug(const char * format, ...);

const char *timeStamp();
void asciiToEbcdic(unsigned char * destino,unsigned char * origem,unsigned int tamanho);
char * cardco_locate_track_data(char *track, int field);

void dateToInteger(char *data, int *dia, int *mes, int *ano);
void logMsg(const char * format, ...);
void integerToDate(int dia, int mes, int ano, char *dataFim);
char *__itoa (unsigned int value, char *string, unsigned int tam);
int calcUltDia(int mes, int ano);
int distancia1(int *dia, int *mes, int *ano, int *ndias);
void somaData(char *data, int dias, char *dataFim);

FILE * dbg;

/* Verifica se o bit esta ligado */
int isBitSet(unsigned char *bitmap, int bit)
{
	/* Procura o byte em questao */
	while (bit > 8)
	{
		bit -= 8;
		++bitmap;
	}

	if ( (*bitmap) & (256 >> bit))
		return -1;
	else
		return 0;
}

/* Liga o bit */
int setBitOn(unsigned char *bitmap, int bit)
{
	/* Procura o byte em questao */
	while (bit > 8)
	{
		bit -= 8;
		++bitmap;
	}
	*bitmap |= 256 >> bit;
}

/* Desliga o bit */
void  setBitOff(unsigned char *bitmap, int bitno)
{
	bitmap [bitno / 8] &= 255 - (1 << (bitno % 8));
}

/* Converte varios formatos de entrada */
int formatIn(unsigned char *src, void *dst, int fmt, int len)
{
	char buf[32];
	char *p;
	char *q;
	char xbuf[50];
	int i, tam;
	int ind; /* indice do bcd onde estah o algarismo corrente */
	int pos_in; /* posicao dentro do byte onde estah o algarismo corrente
	               0 = a esquerda;  1 = a direita */
	char alg; /* algarismo corrente */

	switch (fmt)
	{
	case FMT_CHAR:
		memcpy (dst, src, len);
		p = (char *) dst;
		*(p + len)  = 0;
		debug("field[%d] %s len %d\n", bitno, (char *) dst, len);
		break;
	case FMT_BTOC:
		/* Converte de bcd para char */
		// btoc(src, dst, len);
		tam = bcdtochar(src, (char *) dst, len);

		debug("field[%d] %s len %d\n", bitno, (char *) dst, len);
		len = tam;
		debug("Deslocamento %d\n", len);
		break;
	case FMT_BTOI:
		/* Converte de bcd para inteiro */
		*( (int *) dst ) = btoi(src, len);
		debug("field[%d] %d len %d\n", bitno, *((int *) dst), len);
		len = len/2;
		break;
	case FMT_BTOS:
		/* Converte de bcd para short */
		tam = bcdtochar(src, buf, len);
		*( (short *) dst ) =  (short ) atoi(buf);
		debug("field[%d] %d len %d\n",bitno, *((int *) dst), len);
		len = tam;

		break;
	case FMT_CTOM:
		p = (char *)dst;
		memcpy(p, src, len);
		p[len] = p[len - 1];
		p[len - 1] = p[len - 2];
		p[len - 2] = '.';
		p[len + 1] = '\0';
		q = p;
		while (*q == '0')
		{
			q++;
		}
		strcpy(xbuf, q);
		strcpy(p, xbuf);
		debug("field[%d] %s len %d\n",bitno, (char *) dst, len);
		break;
	case FMT_BTOR:
		len = (len + 1) / 2;
		break;
	case FMT_BTOM:
		/* Converte de bcd para money */
		p = (char *)dst;
		for (i = 0; i < len; i++)
		{
			ind = i / 2;
			pos_in = i % 2;
			if (pos_in == 0)
			{
				alg = ((src[ind] & 0xf0) >> 4) | 0x30;
			}
			else
			{
				alg = (src[ind] & 0x0f) | 0x30;
			}
			p[i] = alg;
		}
		p[len] = p[len - 1];
		p[len - 1] = p[len - 2];
		p[len - 2] = '.';
		p[len + 1] = '\0';
		q = p;
		while (*q == '0')
		{
			q++;
		}
		strcpy(xbuf, q);
		strcpy(p, xbuf);
		len = (len + 1) / 2;
		debug("field[%d] %s len %d\n",bitno, (char *) dst, len);
		break;
	case FMT_BINARY:
		toZoned((char *) dst, (char *) src , 2*len);
		debug("field %s len %d\n", (char *) dst, len);
		break;
	}
	return len;
}

/* Converte varios formatos de saida */
int formatOut(unsigned char *dst, void *src, int fmt, int len)
{
	int cont;
	int ind; /* indice do dst para onde vai o algarismo corrente */
	int pos_in; /* posicao dentro do byte do dst para onde deve ir o algarismo
	               corrente */
	int len_dst;
	int len_src;
	unsigned char *p;

	switch (fmt)
	{
	case FMT_CHAR:
		memcpy ((char *) dst, (char *) src, len);
		debug("field[%d] %s len %d\n", bitno, (char *) dst, len);
		break;
	case FMT_BTOC:
		debug("field[%d] %s len %d\n", bitno, (char *) src, len);
		chartobcd(src,dst, len);
		len = (len + 1)/2;
		break;
	case FMT_BTOI:

		debug("field[%d] %d len %d\n", bitno, *((int *) src), len);
		itob(dst, *(int *) src, len);
		len = (len + 1)/2;
		break;
	case FMT_BTOS:
		debug("field[%d] %d len %d\n", bitno, *((int *) src), len);
		len = itob(dst, *(int *) src, len);
		break;
	case FMT_BTOM:
		debug("field[%d] %s len %d\n",bitno, (char *) src, len);
		len_dst = (len + 1) / 2;
		len_src = strlen(src);
		p = (unsigned char *)src;
		// p[len_src - 3] = p[len_src - 2];
		// p[len_src - 2] = p[len_src - 1];
		// p[len_src - 1] = '\0';
		cont = 0;
		while (p[cont] != '\0')
		{
			ind = cont / 2;
			pos_in = cont % 2;
			if (pos_in == 0)
			{
				dst[ind] = (p[cont] & 0x0f) << 4;
			}
			else
			{
				dst[ind] = dst[ind] | (p[cont] & 0x0f);
			}
			cont++;
		}
		len = len_dst;
		break;
	case FMT_BINARY:
		toBinary(dst, src, 2*len);
		break;
	case FMT_CTOM:
		len_src = strlen(src);
		p = (unsigned char *)src;
		//	p[len_src - 3] = p[len_src - 2];
		//	p[len_src - 2] = p[len_src - 1];
		//	p[len_src - 1] = '\0';
		len = len_src;
		memcpy((char *) dst, src, len);
		debug("field[%d] %s len %d\n", bitno, (char *) dst, len);
		break;
	case FMT_ITOA:
		debug("field[%d] %d len %d\n", bitno, *((int *) src), len);
		sprintf((char *) dst, "%02d",*((int *) src));
		break;
	}
	return len;
}





int btoi(unsigned char * src, int len)
{
	int dst;
	char buf[32];
	char *p;
	int i, pos_in, ind;
	char alg;

	p = (char *)buf;
	for (i = 0; i < len; i++)
	{
		ind = i / 2;
		pos_in = i % 2;
		if (pos_in == 0)
		{
			alg = ((src[ind] & 0xf0) >> 4) | 0x30;
		}
		else
		{
			alg = (src[ind] & 0x0f) | 0x30;
		}
		buf[i] = alg;
	}
	buf[len] = '\0';
	dst = atoi(buf);
	return dst;
}

short btos(unsigned char * src, int len)
{
	short dst;
	char buf[16];
	char alg;
	char *p;
	int i, pos_in, ind;
	p = (char *)buf;
	for (i = 0; i < len; i++)
	{
		ind = i / 2;
		pos_in = i % 2;
		if (pos_in == 0)
		{
			alg = ((src[ind] & 0xf0) >> 4) | 0x30;
		}
		else
		{
			alg = (src[ind] & 0x0f) | 0x30;
		}
		buf[i] = alg;
	}
	buf[len] = '\0';
	dst = (short ) atoi(buf);
	return dst;
}

int btoc(unsigned char * src, void * dst, int len)
{

	char *p;
	int i,pos_in, ind;
	char alg;

	p = (char *) dst;
	for (i = 0; i < len; i++)
	{
		ind = i / 2;
		pos_in = i % 2;
		if (pos_in == 0)
		{
			alg = ((src[ind] & 0xf0) >> 4) | 0x30;
		}
		else
		{
			alg = (src[ind] & 0x0f) | 0x30;
		}
		*p++ = alg;
	}
	*p = '\0';

	return len;
}

/*
** Converte o numero em bcd apontado por src para uma string com 
** caracteres numericos apontada por dst. O parametro len e' o numero
** de algarismos do numero representado.
** Se o numero de algarismos for impar, considera que o lixo e' o 
** primeiro meio byte do buffer.
** O retorno contem o numero de bytes processados. 
*/
int bcdtochar(char *src, char *dst, int len)
{
	register char *s = src;
	register char *d = dst;
	int l;

	if (len & 1) {
		*d = ((*s) & 0x0F) + 0x30;
		++d;
		++s;
		l = len - 1;
	}
	else
		l = len;

	while (l > 0) {
		*d = (((*s) & 0xF0) >> 4) + 0x30;
		d++;
		*d = ((*s) & 0x0F) + 0x30;
		d++;
		s++;
		l -= 2;
	}

	*d = '\0';
	if (len & 1)
		len++;
	return len / 2;
}

/*
** Converte o numero em bcd apontado por src para uma string com 
** caracteres numericos apontada por dst. O parametro len e' o numero
** de algarismos do numero representado.
** Se o numero de algarismos for impar, considera que o lixo e' o 
** ultimo meio byte do buffer.
** O retorno contem o numero de bytes processados. 
*/
int bcdtochar2(char *src, char *dst, int len)
{
	register char *s = src;
	register char *d = dst;
	int l = len;

	while (l > 0) {
		*d = (((*s) & 0xF0) >> 4) + 0x30;
		d++;
		*d = ((*s) & 0x0F) + 0x30;
		d++;
		s++;
		l -= 2;
	}

	if (len & 1)
		--d;

	*d = '\0';
	if (len & 1)
		len++;
	return len / 2;
}
/* Converte de Inteiro para binario */
int itob(char *dst, int src, int len)
{
	char buf[32];
	char fmtstr[10];
	memset(buf, 0, sizeof(buf));
	__itoa(src, buf, len);
	len = chartobcd(buf,dst, len);
	return len;
}
/* Converte de Short para binario */
void stob(char *dst, short src, int len)
{
	itob(dst, (int ) src, len);
}

int chartobcd(char *src, char *dst, int len)
{
	register char *s = src;
	register char *d = dst;
	int l;

	if (len & 1) {
		*d = (*s) - 0x30;
		++d;
		++s;
		l = len - 1;
	}
	else
		l = len;

	while (l > 0) {
		*d = ((*s) - 0x30) << 4;
		s++;
		*d |= (*s) - 0x30;
		d++;
		s++;
		l -= 2;
	}

	if (len & 1)
		len++;
	return len / 2;
}

int chartobcd2(char *src, char *dst, int len)
{
	register char *s = src;
	register char *d = dst;
	int l = len;

	while (l > 1) {
		*d = ((*s) - 0x30) << 4;
		s++;
		*d |= (*s) - 0x30;
		d++;
		s++;
		l -= 2;
	}

	if (len & 1)
		*d = ((*s) - 0x30) << 4;

	if (len & 1)
		len++;
	return len / 2;
}


void mostraBitmap(unsigned char *bitmap)
{
	int len, i;

	len = 8;
	logMsg("\nlen %d buf: \n", len);
	for(i = 0; i < len ; i++)
	{
		logMsg("%02X  ", bitmap[i]);
	}
	logMsg("\n");
}

void mostraBuffer(unsigned char *buffer, int len)
{
	int i;
	char buf[100], aux[10];
	char bufasc[100], auxasc[10];
	int j = 0;

	buf[0] = '\0';
	bufasc[0] = '\0';
	logMsg("len %d buf: \n", len);
	for(i = 0; i < len ; i++)
	{
		j++;
		sprintf(aux, "%02X  ", buffer[i]);
		strcat(buf, aux);
		if (isprint (buffer[i]))
		{
			sprintf(aux, "%c   ", buffer[i]);
		}
		else
		{
			sprintf(aux, ".   ");
		}

		strcat(bufasc, aux);
		if (j == 10)
		{
			logMsg("%s\n", buf);
			buf[0] = '\0';
			j = 0;
			logMsg("%s\n", bufasc);
			bufasc[0] = '\0';
		}
	}
	logMsg("%s\n", buf);
	logMsg("%s\n", bufasc);
}


void dateToInteger(char *data, int *dia, int *mes, int *ano)
{
	/* Pega uma data no formato dd/mm/aaaa e coloca em variaveis
	   inteiras passadas por referencia */

	char aux [10];

	strncpy (aux, &data[0], 2);  aux [2] = '\0';
	*dia = atoi (aux);

	strncpy (aux, &data [3], 2); aux [2] = '\0';
	*mes = atoi (aux);

	strncpy (aux, &data [6], 4); aux [4] = '\0';
	*ano = atoi (aux);
}

void integerToDate(int dia, int mes, int ano, char *dataFim)
{
	/* Pega a tripla (dia, mes, ano) de inteiros e
	   transforma na string "dd/mm/aaaa" */
	char aux [10];

	strncpy (&dataFim[0], __itoa (dia, aux, 2), 2);
	strncpy (&dataFim[3], __itoa (mes, aux, 2), 2);
	strncpy (&dataFim[6], __itoa (ano, aux, 4), 4);
	dataFim[2] = dataFim[5] = '/';
}

char *__itoa(unsigned int value, char *dest, unsigned int tam)
{
	char string[512];
	/* Converte um numero inteiro para string */
	/* Se tam != 0 completa com zeros a esquerda */
	char temp;
	register int i=0;

	do    /* Faz divisoes sucessivas por 10 */
	{
		string[i++] = (char)((value%10)+0x30);
		value = value/10;
	}
	while (value > 0);

	while (i < tam)   /* Completa com zeros a esquerda */
		string [i++] = 0x30;

	string[i]= '\0';

	value = strlen (string);
	i = 0;
	while ( i < (value/2) )    /* Desinverte a string */
	{
		temp = string[value-1-i];
		string[value-1-i] = string[i];
		string[i++] = temp;
	}
	memcpy(dest, string, tam);
	return dest;
}


int calcUltDia(int mes, int ano)
{
	/* Calcula o ultimo dia do mes corrente */
	switch (mes)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12: return 31;

	case 4:
	case 6:
	case 9:
	case 11: return 30;

	case 2:  if ((ano % 4) == 0)
			return 29;
		else
			return 28;
	default: return 0;
	}
}

int distancia1(int *dia, int *mes, int *ano, int *ndias)
{
	/* recebe a referencia para (dia,mes,ano) e ndias
	   e processa uma iteracao, ou seja,
	   calcula distancia entre esta tripla e o ultimo dia do
	   mes corrente. Se a distancia calculada for menor que
	   os ndias, ajusta a data para o primeiro dia util do
	   proximo mes, decrementa ndias e o devolve .
	   Caso contrario, atinge a data desejada, devolvendo 0
	   e a tripla (dia,mes,ano) final.
	*/
	int ud, distancia;

	ud = calcUltDia (*mes, *ano);
	distancia = ud - *dia;

	if (*ndias > distancia)
	{
		*dia = 1;
		(*mes)++;
		if (*mes > 12)
		{
			*mes = 1;
			*ano = *ano + 1;
		}
		*ndias = *ndias - distancia - 1;
	}
	else
	{
		*dia += *ndias;
		*ndias = 0;
	}
	return *ndias;
}

void somaData(char *data, int dias, char *dataFim)
{
	/* Dada uma data no formato string, mais n dias,
	   devolve a nova dataFim tambem no formato string
	   atraves de inumeras iteracoes mensais.
	*/
	int dia, mes, ano, ndias;

	dateToInteger (data, &dia, &mes, &ano);
	ndias = dias;

	while (distancia1 (&dia, &mes, &ano, &ndias))
		;
	integerToDate (dia, mes, ano, dataFim);
}


void dumpMsg(unsigned char *bp, int len, char *msg)
{
	int     i;
	FILE *out;
	out = fopen("fmt.log","a");
	if(msg != NULL)
		fprintf(out,"******************  %s **************\n", msg);
	fprintf(out,"Length: %d\n<", len);

	for(i = 0; i < len; ++i, ++bp)
	{
		fprintf(out,"%02x", ((int ) *bp) & 0xff);
	}
	fprintf(out,">\n");
	fclose(out);
}



void dumpMessage(
        FILE * file,
        const char *sdirecao,
        unsigned char * smensagem,
        int len)
{
#define M 16
	char asciitab[] = {

	                          0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x0C,0x0D,0x00,0x00,
	                          0x00,0x11,0x12,0x13,0x00,0x0A,0x00,0x00,
	                          0x00,0x19,0x00,0x00,0x1C,0x1D,0x1E,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,

	                          0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x63,0x2E,0x3C,0x28,0x2B,0x7C,
	                          0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x21,0x24,0x2A,0x29,0x3B,0x5E,
	                          0x2D,0x2F,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x7C,0x2C,0x25,0x5F,0x3E,0x3F,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,

	                          0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
	                          0x68,0x69,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,
	                          0x71,0x72,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,
	                          0x79,0x7A,0x00,0x00,0x00,0x5B,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x5D,0x00,0x5F,

	                          0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
	                          0x48,0x49,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,
	                          0x51,0x52,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x5C,0x00,0x53,0x54,0x55,0x56,0x57,0x58,
	                          0x59,0x5A,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	                          0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00
	                  };

	int inLine;
	int z;
	int y;
	char line[79];
	char ch;
	int col[16] = {  5, 8, 11, 14, 17, 20, 23, 26,
	                 29, 32, 35, 38, 41, 44, 47, 50 };
	int chl[16] = { 51, 52, 53, 54, 55, 56, 57, 58,
	                60, 61, 62, 63, 64, 65, 66, 67, };
	memset( line,32,78 );
	line[78]=0;
	fprintf (file,
	         "\n%s\n EBCDIC Message: #%02d bytes\n\n", sdirecao, len );
	fprintf (file,
	         "     00.01.02.03.04.05.06.07 08.09.10.11.12.13.14.15");
	fprintf (file, " 01234567 89ABCDEF\n");
	fprintf (file, "     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.--");
	fprintf (file, " ........ ........\n");
	inLine = 0;
	y = 0;
	for (z = 0; z < len; z++)
	{
		sprintf( (line +  col[inLine]),
		         "%02X", smensagem[z]);
		line[col[inLine]+2] = 32;
		ch = asciitab[smensagem[z]];
		line[chl[inLine]+2] = isprint((int)(ch))?ch:'.';
		inLine++;
		if ( inLine != 16) continue;
		inLine = 0;
		sprintf( line, "%04d", y );
		line[4] = 32;
		/* line[5] = 32; */
		fprintf(file,"%s\n", line);
		y = z + 1;
		memset( line, 32, 78);
	} /* for */
	if ( inLine )
	{
		sprintf( line, "%04d", y );
		line[4] = 32;
		fprintf (file, "%s\n", line );
	}
	fprintf (file,
	         "     --.--.--.--.--.--.--.-- --.--.--.--.--.--.--.--");
	fprintf (file,
	         " ........ ........\n");
	fflush(file);
} /* dump_mensagem */

void atoe(unsigned char * destino, unsigned int tamanho)
{
	unsigned char buf[4096];
	asciiToEbcdic(buf, destino, tamanho);
	memcpy(destino, buf, tamanho);
}

void asciiToEbcdic(
        unsigned char * destino,
        unsigned char * origem,
        unsigned int tamanho
)
{

	unsigned char psibyte;
	unsigned int index;
	static  unsigned char ebcdic_tab[256] = {
	                                                /* 0 */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                                                /* 1 */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                                                /* 2 */ 0x40,0x5a,0x7f,0x7b,0x5b,0x6c,0x50,0x7d,
	                                                0x4d,0x5d,0x5c,0x4e,0x6b,0x60,0x4b,0x61,
	                                                /* 3 */ 0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,
	                                                0xf8,0xf9,0x7a,0x5e,0x4c,0x7e,0x6e,0x6f,
	                                                /* 4 */ 0x7c,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,
	                                                0xc8,0xc9,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,
	                                                /* 5 */ 0xd7,0xd8,0xd9,0xe2,0xe3,0xe4,0xe5,0xe6,
	                                                0xe7,0xe8,0xe9,0x4a,0xe0,0x5a,0x5f,0x6d,
	                                                /* 6 */ 0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
	                                                0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,
	                                                /* 7 */ 0x97,0x98,0x99,0xa2,0xa3,0xa4,0xa5,0xa6,
	                                                0xa7,0xa8,0xa9,0xc0,0x6a,0xd0,0xa1,0x00
	                                        };
	index = 0;
	while(tamanho > 0)
	{
		psibyte = origem[index];
		destino[index] = ebcdic_tab[psibyte];
		index++;
		tamanho --;
	}
}

void etoa(
        unsigned char * origem,
        unsigned int tamanho
)
{
	unsigned char buf[4096];
	ebcdicToAscii( buf,origem, tamanho);
	memcpy(origem, buf, tamanho);

}

int  ebcdicToAscii(
        unsigned char * destino,
        unsigned char * origem,
        unsigned int tamanho
)
{

	char asciitab[] = {
	                          0x00,0x00,0x00,0x00,0x00,0x09,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x0C,0x0D,0x00,0x00,
	                          0x00,0x11,0x12,0x13,0x00,0x0A,0x00,0x00,
	                          0x00,0x19,0x00,0x00,0x1C,0x1D,0x1E,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,
	                          0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x63,0x2E,0x3C,0x28,0x2B,0x7C,
	                          0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x21,0x24,0x2A,0x29,0x3B,0x5E,
	                          0x2D,0x2F,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x7C,0x2C,0x25,0x5F,0x3E,0x3F,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,
	                          0x00,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
	                          0x68,0x69,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,
	                          0x71,0x72,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,
	                          0x79,0x7A,0x00,0x00,0x00,0x5B,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x00,0x00,0x00,0x00,0x00,0x5D,0x00,0x5F,
	                          0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
	                          0x48,0x49,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,
	                          0x51,0x52,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x5C,0x00,0x53,0x54,0x55,0x56,0x57,0x58,
	                          0x59,0x5A,0x00,0x00,0x00,0x00,0x00,0x00,
	                          0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	                          0x38,0x39,0x00,0x00,0x00,0x00,0x00,0x00
	                  };


	unsigned char psibyte;
	unsigned int auxind;
	unsigned int index,tam_real;


#define CODDLE  0x10
#define CODSTX  0x02
#define CODETX  0x03
#define SYNC    0x32

#define CODNUL  0X20                            /* codigo nulo */

	/* tamanho = tamanho - 5;    ** 02 27 f5 c2 03 */

	index = 0;
	/* auxind = 4;             **  02 27 f5 c2 */
	auxind = 0;             /* DLE[01] STX[02]  */

	tam_real = tamanho;

	while(tamanho > 0)
	{
		/* converte ebcdic para ascii - ignora 'ordem'*/

		psibyte = origem[auxind];

		/*
		            psibyte = prcv_buffer[auxind];
		            printf("-EBC:[%x]", origem[auxind]);
		*/

		switch(psibyte)         /* EBCDIC */
		{
		case CODDLE:
			tamanho--;   /* 0x10 */
			auxind++;
			if (psibyte = origem[auxind] == CODDLE)
			{
				tamanho--;   /* 0x10 */
				/*
				                    printf("-DLE:[%x]\n", origem[auxind]);
				*/
				auxind++;
				tam_real--;
			}
			else
				if (psibyte = origem[auxind] == CODSTX)
				{
					tamanho--;   /* 0x02 */
					tam_real--;
					/*
					                   printf("-STX:[%x]\n", origem[auxind]);
					*/
					auxind++;
					tam_real--;
				}
				else
					if (psibyte = origem[auxind] == CODETX)
					{
						tamanho--;   /* 0x03 */
						tam_real--;
						/*
						                  printf("-ETX:[%x]\n", origem[auxind]);
						*/
						auxind++;
						tam_real--;
					}
			break;
		case SYNC:
			tamanho--;   /* 0x32 */
			auxind++;
			tam_real--;
			break;
		default:
			destino[auxind] = asciitab[psibyte];
			/*
			                     printf("-ASC:[%c]\n", destino[auxind]);
			*/
			tamanho--;
			index++;
			auxind++;
		}
	}
	return(tam_real);
}

void toZoned( char *dest, char *src, int zonedlen)
{
	register char odd, num;

	for( odd = 0; zonedlen--; odd ^= 0x01, dest++)
	{
		num = (odd) ? *src++ & 0x0F : (*src & 0xF0) >> 4;
		*dest = (num > 9) ? num - 10 + 'A' : num + '0';
	}
}


int toBinary( char *dest, char *src, int zonedlen)
{
	register char odd, num;

	for( odd = 0; zonedlen--; odd ^= 0x01, src++)
	{
		if( *src >= '0' && *src <= '9')
			num = *src - '0';
		else if( *src >= 'A' && *src <= 'F')
			num = *src - 'A' + 10;
		else if( *src >= 'a' && *src <= 'f')
			num = *src - 'a' + 10;
		else
			return( -1);

		if( odd)
			*dest++ |= num;
		else
			*dest = num << 4;
	}
}

void debugOn(const char * filename)
{

	char path[80];
	sprintf(path, "./debug/%s", filename);
	dbg = fopen(path,"a");
}

void debug(const char * format, ...)
{

	char dbgmsg[512];
	va_list ap;

	va_start(ap, format);
	vsprintf(dbgmsg, format,ap);
	va_end(ap);
	// fprintf(dbg, "%s (%s|%d) %s\n", timeStamp(), __FILE__,__LINE__, dbgmsg);
	fprintf(dbg, "%s", dbgmsg);
	// (void ) fputc('\n', dbg);
	fflush(dbg);
	return;
}

void logMsgSetPrefix(char * pfx)
{
	strcpy(prefix,pfx);
}

void logMsg(const char * format, ...)
{

	char logmsg[512];
	char * logPtr;
	va_list ap;
	sprintf(logmsg,"%s [%s] - ", timeStamp(), prefix);
	logPtr = logmsg + strlen(logmsg);
	va_start(ap, format);
	vsprintf(logPtr, format,ap);
	va_end(ap);
	// strcat(logmsg,"\n");
	gravaFila(LOGQ,logmsg, strlen(logmsg));
	return;
}

long cardco_fmtgetdate_from_mmdd(long auxdate)
{
	time_t clock;
	struct tm *tms;
	long dta;
	static char buf[32];
	time(&clock);
	tms = localtime(&clock);
	if(tms->tm_year >= 100) tms->tm_year -= 100;
	dta = tms->tm_year*10000L + auxdate;
	return dta;


}

long cardco_local_currentdate()
{
	time_t clock;
	struct tm *tms;
	long dta;
	static char buf[32];
	time(&clock);
	tms = localtime(&clock);
	if(tms->tm_year >= 100) tms->tm_year -= 100;
	tms->tm_mon + 1;
	sprintf(buf, "%02d%02d%02d", tms->tm_year, tms->tm_mon + 1,
	        tms->tm_mday);
	dta = atol(buf);
	return dta;

}

long cardco_local_currenttime()
{
	time_t clock;
	struct tm *tms;
	long dta;
	static char buf[32];
	time(&clock);
	tms = localtime(&clock);
	sprintf(buf, "%02d%02d%02d\0", tms->tm_hour, tms->tm_min, tms->tm_sec);
	dta = atol(buf);
	return dta;

}

char * cardco_locate_track_data(char *track, int field)
{

	char *p;
	char trackData[50];
	p = track;

	int count = 0;

	if (field == 34)
	{
		while(*p != '=' && *p != 'D' && *p != 'd' && count < 19)
		{
			if(*p == '7' && *(p + 1) == 'e') {
				break;
			}
			p++;
			count++;
		}
	}
	else
	{
		while(*p != '=' && *p != 'D' && *p != 'd' && count < 38)
		{
			if(*p == '7' && *(p + 1) == 'e') {
				break;
			}
			if(*p == '3' && *(p + 1) == 'd') {
				break;
			}
			p++;
			count++;
		}
	}
	strcpy(trackData,p + 1);
	return (p + 1);
}


const char *timeStamp()
{
	time_t clock;
	struct tm *tms;
	static char buf[32];
	time(&clock);
	tms = localtime(&clock);
	if(tms->tm_year >= 100) tms->tm_year -= 100;
	sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d\0", tms->tm_year, tms->tm_mon + 1,
	        tms->tm_mday, tms->tm_hour, tms->tm_min, tms->tm_sec);
	return buf;
}
