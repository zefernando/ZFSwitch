#include "cardco.h"

int decsub(dec_t *num1, dec_t *num2, dec_t *diff)
{
	char buf[50];
	double d1,d2,d3;
	dectochar(num1,buf);
        d1 = atof(buf);
	dectochar(num2,buf);
	d2 = atof(buf);
        d3 = d1 - d2;
        dbltodec(diff,d3);
	return(0);

}

int decsubx(dec_t *num1, dec_t *num2)
{
	char buf[50];
	double d1,d2;
	dectochar(num1,buf);
        d1 = atof(buf);
	dectochar(num2,buf);
	d2 = atof(buf);
        d1 -= d2;
        dbltodec(num1,d1);
	return(0);
}

int decaddx(dec_t *num1, dec_t *num2)
{
	char buf[50];
	double d1,d2;
	dectochar(num1,buf);
        d1 = atof(buf);
	dectochar(num2,buf);
	d2 = atof(buf);
        d2 += d1;
        dbltodec(num2,d2);
	return(0);
}

int decabs(dec_t *num1)
{
	char buf[50];
	double d1,d2;
	dectochar(num1,buf);
        d1 = atof(buf);
        d2 = (double ) 0 - d1;
        dbltodec(num1,d2);
}

int decnmul(dec_t *num1, double d2)
{
	char buf[50];
	double d1;
	dectochar(num1,buf);
        d1 = atof(buf);
	d1 *= d2;
        dbltodec(num1,d1);
	return(0);
}


int decdiv(dec_t *res, dec_t *num1, dec_t *num2)
{
	char buf[50];
	double d1,d2,d3;
	dectochar(num1,buf);
        d1 = atof(buf);
	dectochar(num2,buf);
        d2 = atof(buf);
	d3 = d1/d2;
        dbltodec(res,d3);
	return(0);
}

int decncmp(dec_t *num1, double d2)
{

	char buf[50];
	double d1;
	dectochar(num1,buf);
        d1 = atof(buf);
	if(d1 < d2) return -1;	
	if(d1 == d2) return 0;
	if(d1 > d2 ) return 1;
}

int deccmp(dec_t *num1, dec_t *num2)
{

	char buf[50];
	double d1,d2;
	dectochar(num1,buf);
        d1 = atof(buf);
	logMsg("valor1 %s\n",buf);
	dectochar(num2,buf);
        d2 = atof(buf);
	logMsg("valor2 %s\n",buf);
	if(d1 < d2) return -1;	
	if(d1 == d2) return 0;
	if(d1 > d2 ) return 1;
}

/* Converte dollar para Real */
int conv_dtor(dec_t * num1,dec_t *num2, double taxa)
{
	char buf[50];
	double d1;
	dectochar(num1,buf);
        d1 = atof(buf);
        logMsg("d1 %f\n",d1);
	d1 *= taxa;
	dbltodec(num2,d1);
	return(0);
}

int dectochar(dec_t * num1, char * buf)
{
	strcpy( buf, (char *) num1);
	return 1;	
	
}

int dbltodec(dec_t * num1, double d1)
{
	sprintf((char *) num1, "%f", d1);	
}
