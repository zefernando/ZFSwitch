/*#ident "@(#) bitmap.c 1.00 "*/
/*
 *	Declare bit maps for internal use
 *
 *      Modification History
 *
 *	Who		When		Why
 *	========================================================================
 *
 */

#include <stdio.h>

#include "cardco.h"
#include "extern.h"


unsigned char map100p[16] = {0};
int idx100p[] =
        { 2, 3, 4, 7, 11, 18, 19, 22, 25, 32, 37, 41, 42, 49, -1};

unsigned char map120p[16] = {0};
int idx120p[] =
        { 2, 3, 4, 7, 11, 18, 19, 22, 25, 32, 37, 38, 39, 41, 42, 43, 44, 49, 120, -1};

unsigned char map110p[16] = {0};
int idx110p[] =
        { 2, 3, 4, 7, 11, 19, 25, 32, 37, 38, 39, 49, -1};
/* fields 41 and 42 have to be returned only if present on the request */
/* { 2, 3, 4, 7, 11, 19, 25, 32, 37, 38, 39, 41, 42, 49, -1}; */

unsigned char map130p[16] = {0};
int idx130p[] =
        { 2, 3, 4, 7, 11, 19, 25, 32, 37, 39, 41, 42, 49, -1};

unsigned char map302p[16] = {0};
int idx302p[] =
        { 7, 11, 37, 101, 127, -1};

unsigned char map312p[16] = {0};
int idx312p[] = { 7, 37, 39, -1};

unsigned char map400p[16] = {0};
int idx400p[] =
        { 2, 3, 4, 7, 11, 18, 19, 22, 25, 32, 37, 38, 41, 42, 49, 90, -1};

/* dwk added this bitmap as ncb jam MIP complained of missing bit 39 */
unsigned char map420p[16] = {0};
int idx420p[] =
        { 2, 3, 4, 7, 11, 18, 19, 22, 25, 32, 37, 38, 39, 41, 42, 49, 90, -1};

unsigned char map410p[16] = {0};
int idx410p[] =
        { 2, 3, 4, 7, 11, 32, 37, 39, 49, -1};
/* fields 41 and 42 have to be returned only if present on the request */
/* { 2, 3, 4, 7, 11, 32, 37, 39, 41, 42, 49, -1}; */

unsigned char map620[16] = {0};
int idx620[] = { 7, 11, 33, 60, 63, 100, -1};

unsigned char map630[16] = {0};
int idx630[] = { 7, 11, 33, 39, 63, 100, -1};


unsigned char map800[16] = {0};
int idx800[] = { 7, 37, 70, -1};

unsigned char map810[16] = {0};
int idx810[] = { 7, 37, 70, -1};

struct msgmap msgmap[] = {
	                         {map100p,	idx100p,	100,	MSG_BOTH},
	                         {map100p,	NULL,		101,	MSG_BOTH},
	                         {map120p,	idx120p,	120,	MSG_BOTH},
	                         {map110p,	idx110p,	110,	MSG_BOTH},
	                         {map130p,	idx130p,	130,	MSG_BOTH},
	                         {map400p,	idx400p,	400,	MSG_BOTH},
	                         {map400p,	NULL,		401,	MSG_BOTH},
	                         {map420p,	idx420p,	420,	MSG_BOTH},
	                         {map410p,	idx410p,	410,	MSG_BOTH},
	                         {map620,	idx620,		600,	MSG_BOTH},
	                         {map630,	idx630,		610,	MSG_BOTH},
	                         {map800,	idx800,		800,	MSG_BOTH},
	                         {map810,	idx810,		810,	MSG_BOTH},
	                         {map800,	NULL,		820,	MSG_BOTH},
	                         {map302p,	idx302p,	302,	MSG_BOTH},
	                         {map312p,	idx312p,	302,	MSG_BOTH},
	                         {NULL, NULL}
                         };


unsigned char *find_bitmap(register int msgno, register int type)
{
	register int		i;
	static unsigned char bitmap[20];

	for(i = 0; msgmap[i].map; ++i)
		if(msgmap[i].msgno == msgno && (msgmap[i].type & type))
		{
			memcpy(bitmap, msgmap[i].map, 16);
			return(bitmap);
		}

	return(NULL);
}



create_bitmaps()
{
	int		i;

	for(i = 0; msgmap[i].map; ++i)
		if(msgmap[i].idx)
			make_bitmap(msgmap[i].map, msgmap[i].idx);
}


make_bitmap(register unsigned char *bitmap, register int *index)
{
	int		topbit_set = 0;

	for(; *index != -1; ++index)
	{
		if(*index > 64)
			topbit_set = 1;
		setBitOn(bitmap, *index);
	}

	if(topbit_set)
		setBitOn(bitmap, 0);
}
