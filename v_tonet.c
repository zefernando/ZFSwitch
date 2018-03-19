/*#ident "@(#) v_tonet.c 1.00 18Jun97"*/
/*
 *	Format the switch message to extern external format
 *
 *
 *	Modification History
 *
 *	Who		When		Why
 *	========================================================================
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "cardco.h"
/*
#include "cardco300.h"
#include "cardco500std.h"
*/
#include "extern.h"



cardcoToExtern(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	register	struct 		internalmsg *msg;
	register	unsigned char	*bp;
	struct		cardco500std	*msg500;
	struct		cardco300		*msg300;
	struct		cardco300fileupd	*mupd;
	struct		cardco300pvv	*mpvv;
	char		xbuf[100];
	unsigned	char		*save_bp;
	unsigned 	char		*bitmap;
	char		*p;
	char		*charptr;
	unsigned	char		*p_tr;
	int		i;
//	int		bitno;
	long		j;
	int		is500, is300;
	int		k;
	int		z;
	short           l;
	struct		private_data 	*pd;
	int		fmt_debugorg;

	msg = &cardcoPtr->msg;
	bp  = externmsg->msg;
	bitmap	= externmsg->bitmap;

	pd = (struct private_data *)msg->formatter_use;

	/* Determine message number */
	itob(externmsg->msgno, msg->msgtype, 4);
	msgType = msg->msgtype;
	if (extern800debug ||
	                (msgType != 800 && msgType != 810) ) {
		int	x;

		debug("\n************************* OUTBOUND *******************\n");
		debug("msgno[  0]<%d>\n", msg->msgtype);
		debug("Bitmap: [");
		for(x = 0; x < 8; ++x)
			debug("%02x", bitmap[x]);
		if(isBitSet(bitmap, 1))
			for(x = 8; x < 16; ++x)
				debug("%02x", bitmap[x]);

		debug("]\n");
	}

	if (msg->msgtype / 100 == 5) {
		is500 = 1;
		msg500 = (struct cardco500std *)msg; }
	else
		is500 = 0;

	if (cardcoIs03xx(cardcoPtr)) {
		is300 = 1;
		msg300 = (struct cardco300 *)msg; }
	else
		is300 = 0;

	for(bitno = 1; bitno <= 128; ++bitno) {
		if (!isBitSet(bitmap,bitno)) continue;

		if (bitno > 64)
			if(!isBitSet(bitmap, 1)) break;

		/* =======================================================
		 * the format_in and formatOut() functions have internal
		 * debug calls .  So turn off the fmt_debug flag if recieving
		 * 800/810 message
		 * ======================================================= */
		if (!extern800debug &&
		                (msgType == CARDCO_NETWORK || msgType == CARDCO_NETWORK_RESPONSE) ) {
			/*	save original val to be restored later	*/
			fmt_debugorg = fmt_debug;
			/*
			debug("%s[%d] Original val of fmt_debug is: %d\n",
				__FILE__, __LINE__, fmt_debug);
			*/
			fmt_debug = 0;	/* turn off debugging	*/
		}


		switch(bitno){
		case  1:
			/* secondary bit map */
			/*bp += formatOut(bp, bitmap + 8, FMT_MOVE, 8);*/
			break;

		case  2:
			/* Pan length */
			if (is300)
				p = msg300->pan;
			else
				p = msg->pan;

			/* Skip leading zeros */
			while(*p && *p == '0') ++p;

			/*The pan */
			j = strlen(p); 
			*bp++ = j;
			bp += formatOut(bp , p, FMT_BTOC, j);
			break;

		case  3:
			/* processing code */
			bp += formatOut(bp, &msg->pcode, FMT_BTOI, 6);
			break;

		case  4:
			/* amount of transaction */
			bp += formatOut(bp, &msg->amount, FMT_BTOM, 12);
			break;

		case  5:
			bp += formatOut(bp, &msg->settlement_amount, FMT_BTOM, 12);
			break;

		case  6:
			bp += formatOut(bp, &msg->amount_equiv, FMT_BTOM, 12);
			break;

		case  7:
			/*transmission date & time */
			if (is300)
				j = msg300->trandate % 10000L;
			else
				j = msg->trandate % 10000L;
			bp += formatOut(bp, &j, FMT_BTOI, 4);

			if (is300)
				j = msg300->trantime;
			else
				j = msg->trantime;
			bp += formatOut(bp, &j, FMT_BTOI, 6);

			break;

		case  9:
			bp += formatOut(bp, &msg->settlement_rate, FMT_BTOR, 8);
			break;

		case 10:
			bp += formatOut(bp, &msg->iss_conv_rate, FMT_BTOR, 8);
			break;

		case 11:
			if (is300)
				j = msg300->trace;
			else
				j = msg->trace;

			bp += formatOut(bp, &j, FMT_BTOI, 6);
			externmsg->trace_found = 1;
			break;

		case 12:
			bp += formatOut(bp, &msg->local_time, FMT_BTOI, 6);
			break;

		case 13:
			j = msg->local_date % 10000L;
			bp += formatOut(bp, &j, FMT_BTOI, 4);
			break;

		case 14:
			/*card expiry date */

			if (!isBitSet(bitmap, 34)  && /* track II */
			                !isBitSet(bitmap, 44)     /* track I */) {
				/*place the date into the track2 field */
				p = (char *) msg->track2;
				p++;
				memcpy(xbuf, p, 4);
				xbuf[4] = '\0'; }
			else {
				p = (char *) msg->track2;
				p++;
				memcpy(xbuf, p, 4);
				xbuf[4] = '\0'; }

			j = atoi(xbuf);
			if (j <= 0)
				j = 1111;
			else if (j / 100 < 13)
				j = (j % 100) * 100 + (j / 100);

			bp += formatOut(bp, &j, FMT_BTOI, 4);
			break;

		case 15:
			/*settlement date */

			if (is500)
				j = msg500->settlement_date;
			else
				j = msg->settlement_date;

			j = j % 10000L;
			bp += formatOut(bp, &j, FMT_BTOI, 4);
			break;

		case 16:
			/*date conversion */
			j = msg->iss_conv_date % 10000L;
			bp += formatOut(bp, &j, FMT_BTOI, 4);
			break;


		case 18:
			/*merchant type */
			bp += formatOut(bp, &msg->merchant_type, FMT_BTOS, 4);
			break;

		case 19:
			bp += formatOut(bp, &msg->acq_country, FMT_BTOS, 4);
			break;

		case 20:
			/*PAN extended country code */
			/*	Not used */
			bp += 2;
			break;

		case 21:
			/*forwarding country code */
			bp += 2;
			break;

		case 22:
			bp += formatOut(bp, &msg->pos_entry_code, FMT_BTOS, 4);
			break;

		case 23:
			bp += formatOut(bp, &msg->card_seqno, FMT_BTOS, 4);
			break;

		case 25:
			bp += formatOut(bp, &msg->pos_condition_code, FMT_BTOS, 2);
			break;

		case 26:
			*bp++ = msg->pos_pin_cap_code;
			if (extern800debug ||
			                (msgType != CARDCO_NETWORK &&
			                 msgType != CARDCO_NETWORK_RESPONSE) )
				debug(" in[ 26]<%d>\n", msg->pos_pin_cap_code);
			break;

		case 28:
			bp += 9; /* national switching only: Not supported */
			break;

		case 32:
			if (pd->have_large_bin)
				p = (char*) pd->large_bin;
			else
				if (is500)
					p = msg500->acquirer;
				else
					p = msg->acquirer;

			for(; *p == '0'; ++p);


			j = strlen(p);
			*bp++ = j; /* length sub-subfield */
			bp += formatOut(bp, p, FMT_BTOC, j);
			break;

		case 33:
			if (is500)
				p = msg500->forward_inst;
			else
				p = msg->originator;

			for(; *p == '0'; ++p);
			j = strlen(p);
			*bp++ = j; /* length sub-subfield */
			bp += formatOut(bp, p, FMT_BTOC, j);
			break;

		case 34:
			/* PAN extended:	Not used */
			break;

		case 35:
			/* Track II */
			j = strlen(msg->track2);
			*bp++ = j;
			bp += formatOut(bp, msg->track2, FMT_BTOC, j);
			break;

		case 37:
			/* Convert to ascii first and then convert */
			/* retrieval ref. no. */

			if (is300)
				p = msg300->refnum;
			else
				p = msg->refnum;

			for(; isspace(*p); ++p) *p = '0';
			for(; isdigit(*p); ++p);

			*p = '\0';

			if (is300)
				sprintf(xbuf, "%012s", msg300->refnum);
			else
				sprintf(xbuf, "%012s", msg->refnum);

			formatOut(bp, xbuf, FMT_CHAR, 12);
			atoe(bp, 12);
			bp += 12;
			break;

		case 38:
			/*	convert to ascii first */
			formatOut(bp, msg->authnum, FMT_CHAR, 6);
			atoe(bp, 6);
			bp += 6;
			break;

		case 39:
			/* Convert to ascii first and then convert */
			if (is300)
				j = msg300->respcode;
			else
				j = msg->respcode;

			formatOut(bp, &j, FMT_ITOA, 2);
			atoe(bp, 2);
			bp += 2;
			break;

		case 40:
			/* service restriction code:	take from card */
			break;

		case 41:
			/* terminal id */
			/* Convert to ascii first */
			formatOut(bp, msg->termid, FMT_CHAR, 8);
			atoe(bp, 8);
			dumpMsg(bp, 8, "TERMID");
			bp += 8;
			break;

		case 42:
			/* Card acceptor id */
			/* Convert to ascii first */
			formatOut(bp, msg->termloc, FMT_CHAR, 15);
			atoe(bp, 15);
			bp += 15;
			break;

		case 43:
			/* acceptor name and location */
			formatOut(bp, msg->acceptorname, FMT_CHAR, 40);
			atoe(bp, 40);
			bp += 40;
			break;

		case 44:
			/* convert to ascii first */
			/* additional response data */
			/* under extern this is the reversal reason code */

			/*
			j = strlen(msg->addresponse);
			*bp++ = j;
			formatOut(bp, msg->addresponse, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			*/

			/* Changed to hard code */
			*bp++ = 2;
			formatOut(bp, "01", FMT_CHAR, 2); /* timed out by switch */
			atoe(bp, 2);
			bp += j;

			break;

		case 45:
			/* track I data */
			/* We only use track II data */
			j = strlen(msg->track2);
			*bp++ = j;	/* length sub-field */
			formatOut(bp, msg->track2, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			break;

		case 48:
			/* Private data */
			if(msg->acceptorname[4] == 'C' ) {
				*bp++ = 60;
			}
			else {
				*bp++ = 36;
			}
			
                        if(msg->acceptorname[4] != 'D')
                        {		
                         formatOut(bp,&msg->aval_balance, FMT_CTOM,12);
			 atoe(bp,12);
			 bp += 12;
                        }    
			
                        
			formatOut(bp,&msg->ledger_balance, FMT_CTOM,12);
			atoe(bp,12);
			bp += 12;
			
                        if(msg->acceptorname[4] == 'C' ) {
				formatOut(bp,&msg->settlement_amount, FMT_CTOM,12);
				atoe(bp,12);
				bp += 12;
			}
			sprintf(xbuf,"%06ld", msg->settlement_date);
			formatOut(bp,xbuf,FMT_CHAR,6);
			atoe(bp,6);
			bp += 6;
			formatOut(bp,&msg->fee,FMT_CHAR,6);
			atoe(bp,6);
			bp += 6;
			formatOut(bp,&msg->dth_upd, FMT_CHAR,12);
			atoe(bp,12);
			bp += 12;
			break;

		case 49:
			/* currency code */
			bp += formatOut(bp, &msg->acq_currency_code, FMT_BTOS, 3);
			dumpMsg(bp -2,2,  "CURRENCY");
			break;

		case 50:
			if (is500)
				bp += formatOut(bp, &msg500->currency_code, FMT_BTOS, 3);
			else
				bp += formatOut(bp, &msg->settlement_code, FMT_BTOS, 3);
			break;

		case 51:
			/* currency code */
			bp += formatOut(bp, &msg->iss_currency_code, FMT_BTOS, 3);
			break;

		case 52:
			/* PIN */
			/* This is 8 bytes of binary data: */
			/* What we need at the switch is   */
			/* 16 bytes of display hex values  */
			bp += formatOut(bp, msg->pin, FMT_BINARY, 8);
			externmsg->pin_found = 1;
			break;

		case 53:
			/* security related info */
			*bp++ = 0x20;
			*bp++ = 1;
			*bp++ = 1;
			*bp++ = 1; /*msg->pin_index;*/
			memset(bp, 0, 4);
			bp += 4;
			break;

		case 59:
			/* national geographic area */
			j = strlen(msg->pos_geo_loc);
			*bp++ = j;
			formatOut(bp, msg->pos_geo_loc, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			break;

		case 60:
			/* length subfield */
			*bp++ = 2;
			bp += formatOut(bp, &msg->pos_cap_code, FMT_BTOS, 2);
			break;

		case 61:
			/* amount other (charge back) */
			*bp++ = 12;			/*	length sub-field */
			bp += formatOut(bp, &msg->cash_back, FMT_BTOM, 12);
			break;

		case 62:
			*bp++ = pd->length62;
			bitmp62 [0] = pd->bitmp621;
			bp += formatOut( bp, bitmp62, FMT_MOVE ,8);
			if (pd->bitmp621 & 0x80) {
				/* Authorization Characteristics Indicator */
				formatOut(bp, &msg->pos_pin_cap_code, FMT_CHAR, 1);
				atoe(bp,1);
				bp++; }

			/* Transaction Identifier */
			if (pd->bitmp621 & 0x40) {
				formatOut(bp, &msg->storeid, FMT_BTOI, 7);
				bp += 4;
				formatOut(bp, &msg->lane, FMT_BTOI, 8);
				bp += 4; }

			if (pd->bitmp621 & 0x20) {
				/* Validation Code */
				formatOut (bp, pd->validation_code, FMT_CHAR,4);
				atoe(bp,4);
				bp += 4; }
			break;

		case 63:
			save_bp = bp;		bp++;
			p = v_NetworkIdBitmap;	/*	bit map */
			memcpy(bp, v_NetworkIdBitmap, 3);
			bp += 3;
			k = 0;

			if (isBitSet(p, 0)) {
				j = msg->serial_1 / 10000L;
				bp += formatOut(bp, &j, FMT_BTOI, 4); }

			if (isBitSet(p, 1)) {
				j = msg->life_cycle / (60 * 60);
				bp += formatOut(bp, &j, FMT_BTOI, 4); }

			if (isBitSet(p, 2))
				bp += formatOut(bp, &msg->revcode, FMT_BTOI, 4);

			if (isBitSet(p, 3))
				/* STIP Code */
				bp += formatOut(bp, &msg->revcode, FMT_BTOI, 4);

			if (isBitSet(p, 4))
				bp += formatOut(bp, &msg->serial_2, FMT_BTOI, 6);

			if (isBitSet(p, 5))
				/* chargeback information */
				bp += 7;

			if (isBitSet(p, 6)) {
				memset(bp, 0, 8);
				*bp++ = (msg->serial_1 & 0xf);
				bp += 7; }

			if (isBitSet(p, 7))
				/* business id */
				bp += 4;

			if (isBitSet(p, 8))
				bp += 12;

			/* now determine length of field */
			*save_bp = bp - save_bp;
			break;

		case 64:
			/* MAC */
			externmsg->mac_found = 1;
			bp += formatOut(bp, msg->mac.key, FMT_BINARY, 8);
			break;

		case 66:
			/* settlement code */
			bp += formatOut(bp, &msg500->respcode, FMT_BTOI, 1);
			break;

		case 68:
			bp += 2;
			break;

		case 69:
			bp += 2;
			break;

		case 70:
			/* network management code */
			bp += formatOut(bp, &msg->netcode, FMT_BTOI, 3);
			break;

		case 73:
			bp += 3;		/*	date action */
			break;

		case 74:
			bp += formatOut(bp, &msg500->num_credit, FMT_BTOI, 10);
			break;


		case 75:
			bp += formatOut(bp, &msg500->num_credit_rev, FMT_BTOI, 10);
			break;

		case 76:
			bp += formatOut(bp, &msg500->num_debit, FMT_BTOI, 10);
			break;

		case 77:
			bp += formatOut(bp, &msg500->num_debit_rev, FMT_BTOI, 10);
			break;


		case 78:
			bp += formatOut(bp, &msg500->num_transfer, FMT_BTOI, 10);

		case 79:
			bp += formatOut(bp,&msg500->num_transfer_rev, FMT_BTOI, 10);
			break;


		case 80:
			bp += formatOut(bp, &msg500->num_inquiry, FMT_BTOI, 10);
			break;

		case 81:
			bp += formatOut(bp, &msg500->num_auth, FMT_BTOI, 10);
			break;

		case 82:
			bp += formatOut(bp,&msg500->fee_cr_processing,FMT_BTOM, 12);
			break;

		case 83:
			bp += formatOut(bp,&msg500->fee_cr_transaction,FMT_BTOM,12);
			break;

		case 84:
			bp += formatOut(bp,&msg500->fee_db_processing,FMT_BTOM, 12);
			break;

		case 85:
			bp += formatOut(bp,&msg500->fee_db_transaction,FMT_BTOM,12);
			break;

		case 86:
			bp += formatOut(bp, &msg500->amt_credit, FMT_BTOM, 16);
			break;

		case 87:
			bp += formatOut(bp, &msg500->amt_credit_rev, FMT_BTOM, 16);
			break;

		case 88:
			bp += formatOut(bp, &msg500->amt_debit, FMT_BTOM, 16);
			break;

		case 89:
			bp += formatOut(bp, &msg500->amt_debit_rev, FMT_BTOM, 16);
			break;

		case 90:
			/* original data elements */
			/* Convert to ascii first */
			bp += formatOut(bp, &msg->origmsg, FMT_BTOI, 4);
			bp = bp + 3 + 2 + 3 + 11;
			/*
			bp += formatOut(bp, &msg->origtrace, FMT_BTOI, 6);
			j = msg->origdate % 10000;
			bp += formatOut(bp, &j, FMT_BTOI, 4);
			bp += formatOut(bp, &msg->origtime, FMT_BTOI, 6);
			bp += 11;
			*/
			break;

		case 91:
			formatOut(bp, "5", FMT_CHAR, 1);
			atoe(bp, 1);
			++bp;
			break;

		case 92:
			formatOut(bp, msg300->file_security_code, FMT_CHAR, 2);
			atoe(bp, 2);
			bp += 2;
			break;

		case 93:
			/* always '00002' */
			bp += 5;
			break;

		case 94:
			break;

		case 95:
			formatOut(bp, &msg->aval_balance, FMT_CTOM, 12);
			atoe(bp, 12);
			bp += 12;
			bp += (42 - 12);
			externmsg->replacement_amount = 1;
			break;

		case 96:
			break;

		case 97:
			/* amount net settlement */
			save_bp = bp;		bp++;
			if (decncmp(&msg500->amt_netset, (double)0) < 0) {
				decnmul(&msg500->amt_netset, (double)(-1));
				*save_bp = 'D'; }
			else
				*save_bp = 'C';

			bp += formatOut(bp, &msg500->amt_netset, FMT_CTOM, 16);

			break;

		case 99:
			/* settlement agent */
			if (!is500) break;

			p = msg500->settlement_agent;
			for(; *p == '0'; ++p);
			j = strlen(p);
			*bp++ = j;
			bp += formatOut(bp, p, FMT_BTOC, j);
			break;

		case 100:
			/* forward institution */
			if (is500)
				p = msg500->issuer;
			else
				p = msg->issuer;

			for(; *p == '0'; ++p) ;
			j = strlen(p);
			*bp++ = j;
			bp += formatOut(bp, p, FMT_BTOC, j);
			break;

		case 101:
			/* file name */
			j = strlen(msg300->file_name);
			*bp++ = j;
			formatOut(bp, msg300->file_name, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			break;

		case 102:
			/* check verification: 	account # 1 */
			j = strlen(msg->acctnum);
			*bp++ = j;
			formatOut(bp, msg->acctnum, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			break;

		case 103:
			break;

		case 120:
			*bp++ = 1;
			bp += formatOut(bp, &msg->origmsg, FMT_BTOI, 4);
			break;

		case 121:
			break;

		case 122:
			break;

		case 123:
			/* record data */
			/* AVS information */
			j = strlen(msg->addresponse);
			*bp++ = j;
			formatOut(bp, msg->addresponse, FMT_CHAR, j);
			atoe(bp, j);
			bp += j;
			break;

		case 127:
			charptr = (char*) bp++;
			switch(cardcoGetProcessCode(msg300->pcode)){
			case	CC_300_FILE_UPDATE:
				mupd = (struct cardco300fileupd *)msg300->action;

				xbuf[0] = mupd->updcode; xbuf[1] = '\0';
				formatOut(bp, xbuf, FMT_CHAR, 1);
				atoe(bp, 1);	++bp;

				/* Pan length */
				p = mupd->pan;
				/*Skip leading zeros */
				while(*p && *p == '0') ++p;

				/* The pan */
				j = formatOut(bp + 1, p, FMT_BTOC, 0);
				*bp++ = strlen(p);
				bp += j;

				j = mupd->purge_date / 100 % 10000;
				bp += formatOut(bp, &j, FMT_BTOI, 4);

				logMsg("mupd->acton'%s'\n", mupd->action);
				mupd->action[2] = '\0';
				formatOut(bp, mupd->action, FMT_CHAR, 2);
				atoe(bp, 2);
				bp += 2;

				mupd->region[9] = '\0';
				formatOut(bp, mupd->region, FMT_CHAR, 9);
				atoe(bp, 9);
				bp += 9;

				bp +=formatOut(bp,mupd->special_processing,FMT_BTOC,3);

				*charptr = (char*) bp - charptr - 1;
				break;


			case	CC_300_PVV_UPDATE:
				mpvv = (struct cardco300pvv *)msg300->action;

				xbuf[0] = mpvv->updcode; xbuf[1] = '\0';
				formatOut(bp, xbuf, FMT_CHAR, 1);
				atoe(bp, 1);	++bp;

				/* Pan length */
				p = mpvv->pan;
				/* Skip leading zeros */
				while(*p && *p == '0') ++p;

				/* The pan */
				j = formatOut(bp + 1, p, FMT_BTOC, 0);
				*bp++ = strlen(p);
				bp += j;

				j = mpvv->purge_date / 100 % 10000;
				bp += formatOut(bp, &j, FMT_BTOI, 4);

				mpvv->algo[2] = '\0';
				j = atoi(mpvv->algo);
				bp += formatOut(bp, &j, FMT_BTOI, 2);

				mpvv->security_info[6] = '\0';
				j = atoi(mpvv->security_info);
				bp += formatOut(bp, &j, FMT_BTOI, 6);
				memset(bp, 0, 7);		bp += 7;

				*charptr = (char*) bp - charptr - 1;
			default:
				break;
			}

			break;

		case 128:
			break;
		}

		/* =======================================================
		 * the format_in and formatOut() functions have internal
		 * debug calls. So turn off the fmt_debug before these 
		 * function calls if you are recieving 800/810 messages .
		 * but now restore the old value
		 * 800/810 message
		   ======================================================= */
		if (!extern800debug &&
		                (msgType == CARDCO_NETWORK ||
		                 msgType == CARDCO_NETWORK_RESPONSE) ) {
			fmt_debug = fmt_debugorg; /* restore to original val */ }

	}

	return((char *)bp - (char *)externmsg->header);

}
