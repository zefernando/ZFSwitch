/*#ident "@(#) v_data.c 1.00 18Jun97"*/
/*
 *	Globals
 *
 *      Modification History
 *
 *      Who             When            Why
 *      ========================================================================
 *
 */



#include <stdio.h>
#include "cardco.h"
#include "extern.h"


int		mbid_in  = (-1);
int		mbid_out = (-1);
int		pid      = (-1);
int		bitno = 0;

struct		cardcoexternmsg	externmsg  = {0};		/*	message pointers */
struct		cardco		cardcomsg = {0,};
unsigned char	externbuf[EXT_MESSAGE_LEN] = {0};
unsigned char	source_station_id[3]      = {0};
char		*argv0;
short		extern_currency = 840;

bin_t		MIPid;

unsigned char	bitmp62 [8];

char		v_NetworkIdBitmap[8];
char		prefix[128];
int		extern_support300 = 1;	

/* do not dump 800 type messages to the extern.debug file */
int		fmt_debug = 0;
int		extern800debug = 1 ;
int		msgType;
FILE		* dbg;

