/*#iden1 "@(#) v_tocardco.c 1.00 21Mar05"*/
/*
 *	Format the extern external message to internal CardCo format
 *
 *
 *	Modification History
 *
 *	Who		When		Why
 *	========================================================================
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "cardco.h"
#include "cardco500std.h"
#include "extern.h"



externToCardCo(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	register	struct internalmsg	*msg;
	register	unsigned char		*bp;
	struct		cardco500std		*msg500;
	struct		cardco300		*msg300;
	char		xbuf[50];
	unsigned	char		*bitmap;
	char		*p;
	int		i;
	int		z;
//	int		bitno;
	long		j, k, n;
	long		errcode;
	long		fieldlen, auxdate;
	
	int		is300;
	int		is500;
	char		*filename = (char *) NULL;	
	struct		cardco300fileupd	*mupd;
	struct		cardco300pvv	*mpvv;
	struct		private_data	*pd;
	int		fmt_debugorg;

	msg 	= &cardcoPtr->msg;
	bp  	= externmsg->msg;
	bitmap	= externmsg->bitmap;


	pd = (struct private_data *)msg->formatter_use;
	/* length62 will say if there was a field 62 in the msg */
	pd->length62 = 0;

	/* Determine message number */
	msg->msgtype = btoi(externmsg->msgno, 4);
	if(msg->msgtype == 302 || msg->msgtype == 312)
		if(extern_support300 == 0)
			return(0);

	if( (extern800debug) ||
	                (msgType != CARDCO_NETWORK && msgType != CARDCO_NETWORK_RESPONSE) )
	{
		int	x;
		debug("msgno[  0]<%d>\n", msg->msgtype);
		debug("Bitmap: [");
		for(x = 0; x < 8; ++x)
			debug("%02x", bitmap[x]);
		if(isBitSet(bitmap, 1))
			for(x = 8; x < 16; ++x)
				debug("%02x", bitmap[x]);

		debug("]\n");
	}

	if(msg->msgtype / 100 == 5)
	{
		is500 = 1;
		msg500 = (struct cardco500std *)msg;
	}
	else
		is500 = 0;

	if(cardcoIs03xx(cardcoPtr))
	{
		is300 = 1;
		msg300 = (struct cardco300 *)&cardcoPtr->msg;
	}
	else
		is300 = 0;


	for(bitno = 1; bitno <= 128; ++bitno) {
		if(!isBitSet(bitmap,bitno)) continue;

		if(bitno > 64)
			if(!isBitSet(bitmap, 1)) break;

		/* =======================================================
		 * the formatIn and format_out() functions have internal
		 * debug calls.So turn off the fmt_debug flag if recieving
		 * 800/810 message
		 * ======================================================= */
		if (!extern800debug &&
		                (msgType == CARDCO_NETWORK || msgType == CARDCO_NETWORK_RESPONSE)){
			/* save original val to be restored laster	*/
			fmt_debugorg = fmt_debug;
			fmt_debug = 0;	/* turn off debugging */ 
		}

		switch(bitno){
		case  1:
			/* secondary bit map */
			bp += 8;
			/*bp += formatIn(bp, bitmap + 8, FMT_MOVE, 8);*/
			break;

		case  2:
			/* The pan */
			fieldlen = *bp++;
			bp += formatIn(bp, msg->pan, FMT_BTOC, fieldlen);
			break;

		case  3:
			/* processing code */
			bp += formatIn(bp, &msg->pcode, FMT_BTOI, 6);
			/*
			 * added code bellow to try to avoid
			 * sending corrupted data to the switch
			 */
			if ((!cardcoIs03xx(cardcoPtr)) && (extern_make_cardcopcode(cardcoPtr) < 0) )
				return(-1);
			break;

		case  4:
			/* amount of transaction */
			// bp += formatIn(bp, &msg->amount, FMT_BTOM, 12);
			bp += formatIn(bp, &msg->amount, FMT_BTOC, 12);
			break;

		case  5:
			bp += formatIn(bp, &msg->settlement_amount, FMT_BTOM, 12);
			break;

		case  6:
			bp += formatIn(bp, &msg->amount_equiv, FMT_BTOM, 12);
			break;

		case  7:
			/* transmission date & time */
			bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
			auxdate = cardco_fmtgetdate_from_mmdd(auxdate);
			if(is300)
				msg300->trandate = auxdate;
			else
				msg->trandate = auxdate;
			bp += formatIn(bp, &auxdate, FMT_BTOI, 6);
			if(is300)
				msg300->trantime = auxdate;
			else
				msg->trantime = auxdate;
			break;

		case  9:
			bp += formatIn(bp, &msg->settlement_rate, FMT_BTOR, 8);
			break;

		case 10:
			bp += formatIn(bp, &msg->iss_conv_rate, FMT_BTOR, 8);
			break;

		case 11:
			bp += formatIn(bp, &auxdate, FMT_BTOI, 6);
			if(is300)
				msg300->trace = auxdate;
			else
				msg->trace = auxdate;

			externmsg->trace_found = 1;
			break;

		case 12:
			bp += formatIn(bp, &msg->local_time, FMT_BTOI, 6);
			break;

		case 13:
			bp += formatIn(bp, &msg->local_date, FMT_BTOI, 4);
			msg->local_date= cardco_fmtgetdate_from_mmdd(msg->local_date);
			break;

		case 14:
			/* card expiry date */
			bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
			/*
			 * added code bellow to try to avoid
			 * sending corrupted data to the switch
			 * Has to be reviewed before year 2016
			 */
			if ( (auxdate % 100) > 12) return (-1);
			if ( (auxdate % 100) == 0) return (-1);
			if ( (auxdate / 100) > 99) return (-1);
			if(!isBitSet(bitmap, 34)  && /* track II */
			                !isBitSet(bitmap, 44)	   /* track I  */) {
				/* place the date into the track2 field */
				sprintf(msg->track2, "=%04d", auxdate);
			}
			break;

		case 15:
			/* settlement date */
			bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
			auxdate = cardco_fmtgetdate_from_mmdd(auxdate);

			if(is500)
				msg500->settlement_date = auxdate;
			else
				msg->settlement_date = auxdate;
			break;

		case 16:
			/* date conversion */
			bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
			msg->iss_conv_date = cardco_fmtgetdate_from_mmdd(auxdate);
			break;


		case 18:
			/* merchant type */
			bp += formatIn(bp, &msg->merchant_type, FMT_BTOS, 4);
			break;

		case 19:
			bp += formatIn(bp, &msg->acq_country, FMT_BTOS, 4);
			break;

		case 20:
			/* PAN extended country code: Not used */
			bp += 2;
			break;

		case 21:
			/* forwarding country code */
			bp += 2;
			break;

		case 22:
			/* point of service entry mode */
			bp += formatIn(bp, &msg->pos_entry_code, FMT_BTOS, 4);
			// msg->pos_entry_code = msg->pos_entry_code;
			break;

		case 23:
			bp += formatIn(bp, &msg->card_seqno, FMT_BTOS, 4);
			break;

		case 24:
			break;

		case 25:
			bp += formatIn(bp, &msg->pos_condition_code, FMT_BTOS, 2);
			break;

		case 26:
			msg->pos_pin_cap_code = *bp++;
			if( (extern800debug) ||
			                (msgType != CARDCO_NETWORK &&
			                 msgType != CARDCO_NETWORK_RESPONSE) )
				debug(" in[ 26]<%d>\n", msg->pos_pin_cap_code);
			break;

		case 28:
			bp += 9; /* national switching only: Not supported */
			break;

		case 32:
			fieldlen = *bp++; /*	length sub-subfield */
			bp += formatIn(bp, xbuf, FMT_BTOC, fieldlen);
			pd->have_large_bin = 0;
			if( fieldlen > 9) {
				/* This acquirer needs special attention */
				memcpy(pd->large_bin, xbuf, strlen(xbuf)+1);
				pd->have_large_bin = 1;
				strcpy(xbuf, "0000000004"); 
			}

			if(is500)
				strcpy(msg500->acquirer, xbuf);
			else
				strcpy(msg->acquirer, xbuf);
			break;

		case 33:
			fieldlen = *bp++;  /* length sub-subfield */
			bp += formatIn(bp, xbuf, FMT_BTOC, fieldlen);

			if(is500)
				strcpy(msg500->forward_inst, xbuf);
			else
				strcpy(msg->originator, xbuf);
			break;

		case 34:
			/* PAN extended: Not used */
			fieldlen = *bp++;
			bp += fieldlen;
			break;

		case 35:
			/* Track II */
			fieldlen = *bp++;
			bp += formatIn(bp, msg->track2, FMT_BTOC, fieldlen);
			if(fieldlen & 1) /* If Odd number, remove leading zero */ {
				memmove(msg->track2, msg->track2+1, fieldlen);
				msg->track2[fieldlen] = '\0';
			}
			break;

		case 37:
			/* Convert to ascii first and then convert */
			etoa(bp, 12);
			/* retrieval ref. no. */
			bp += formatIn(bp, xbuf, FMT_CHAR, 12);
			/* remove trailing blanks */
			p = strchr(xbuf, ' ');
			if(p != NULL) *p = '\0';
			if(is300)
				strcpy(msg300->refnum, xbuf);
			else
				strcpy(msg->refnum, xbuf);
			break;

		case 38:
			/* convert to ascii first */
			etoa(bp, 6);
			bp += formatIn(bp, msg->authnum, FMT_CHAR, 6);
			break;

		case 39:
			/* Convert to ascii first and then convert */
			etoa(bp, 2);
			bp += formatIn(bp, &j, FMT_ATOI, 2);
			if(is300)
				msg300->respcode = j;
			else
				msg->respcode = j;
			break;

		case 40:
			/* service restriction code: take from card */
			bp += 3;
			break;

		case 41:
			/* terminal id */
			/* Convert to ascii first */
			etoa(bp, 8);
			bp += formatIn(bp, msg->termid, FMT_CHAR, 8);
			break;

		case 42:
			/* Card acceptor id */
			/* Convert to ascii first */
			etoa(bp, 15);
			bp += formatIn(bp, msg->termloc, FMT_CHAR, 15);
			break;

		case 43:
			/* acceptor name and location */
			etoa(bp, 40);
			bp += formatIn(bp, msg->acceptorname, FMT_CHAR, 40);
			break;

		case 44:
			/* convert to ascii first */
			/* additional response data */
			/* under extern this is the reversal reason code */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			bp += formatIn(bp, msg->addresponse, FMT_CHAR, fieldlen);
			break;

		case 45:
			/* track I data */
			/* We only use track II data */
			fieldlen = *bp++; /* length sub-field */
			etoa(bp, fieldlen);
			bp += formatIn(bp, msg->track2, FMT_CHAR, fieldlen);
			break;

		case 48:
			/* Private data: We skip it */
			fieldlen = *bp++;
			if(is300) {
				k = 0;
				p = msg300->action;
				for(k = 0; k < fieldlen; ) {
					/* get error code */
					bp += formatIn(bp, &errcode, FMT_BTOI, 4);
					k += 2;
					p += sprintf(p, "%04d", errcode);
					if(errcode == 530 || errcode == 538 ||
					                errcode == 568)
						*p++ = '@';	/* end of field */
					else {
						/* Get the track length and info */
						n = *bp++;	++k;
						n = formatIn(bp, p, FMT_BTOC, n);
						bp += n;	k += n;
						p = p + strlen(p);
						*p++ = '@'; 
					}
				}/*for*/

				*p++ = '\0';
			}
			else
				bp += fieldlen;
			break;

		case 49:
			/* currency code */
			bp += formatIn(bp, &msg->acq_currency_code, FMT_BTOS, 3);
			break;

		case 50:
			if(is500)
				bp += formatIn(bp, &msg500->currency_code, FMT_BTOS, 3);
			else
				bp += formatIn(bp, &msg->settlement_code, FMT_BTOS, 3);
			break;

		case 51:
			/* currency code */
			bp += formatIn(bp, &msg->iss_currency_code, FMT_BTOS, 3);
			break;

		case 52:
			/* PIN */
			/* This is 8 bytes of binary data: */
			/* What we need at the switch is   */
			/* 16 bytes of display hex values  */
			bp += formatIn(bp, msg->pin, FMT_BINARY, 8);
			externmsg->pin_found = 1;
			break;

		case 53:
			/* security related info */
			msg->format_code[0] = *bp++;	/* security format */
			msg->pin_algo[0]    = *bp++;	/* pin encryp: ANSI */
			msg->pin_algo[1]    = *bp++;	/* pin block */
			msg->pin_index      = *bp++;
			msg->mac_index      = 1;	/* always 1 */
			bp += 4;
			break;

		case 54:
			/* returned balance */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			bp += formatIn(bp, xbuf, FMT_CHAR, 2);
			bp += formatIn(bp, xbuf, FMT_CHAR, 2);
			bp += formatIn(bp, xbuf, FMT_CHAR, 4);
			bp += formatIn(bp, xbuf, FMT_CHAR, 1);
			bp += formatIn(bp, &msg->aval_balance, FMT_CTOM, 12);
			if(fieldlen > 21) {
				bp += formatIn(bp, xbuf, FMT_CHAR, 2);
				bp += formatIn(bp, xbuf, FMT_CHAR, 2);
				bp += formatIn(bp, xbuf, FMT_CHAR, 4);
				bp += formatIn(bp, xbuf, FMT_CHAR, 1);
				bp += formatIn(bp, &msg->cash_back, FMT_CTOM, 12);
			}

			break;

		case 59:
			/* national geographic area */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			bp += formatIn(bp, msg->pos_geo_loc, FMT_CHAR, fieldlen);
			break;

		case 60:
			/* length subfield */
			++bp;
			bp += formatIn(bp, &msg->pos_cap_code, FMT_BTOS, 2);
			break;

		case 61:
			/* amount other (charge back) */
			fieldlen = *bp++; /* length sub-field */

#ifdef MULTICURRENCY
			/* other amount, transaction currency */
			bp += 6;	/* ignore these bytes */

			/* other amount, cardholder billing */
			bp += formatIn(bp, &msg->cash_back, FMT_BTOM, 12);

			/* other amount, replacement billing */
			bp += formatIn( bp, &msg->fee, FMT_BTOM, 12);

#else
			bp += formatIn(bp, &msg->cash_back, FMT_BTOM, 12);
			if(fieldlen > 12) {
				fieldlen = (fieldlen - 12)/2;
				bp += fieldlen; }
#endif
			break;


		case 62:
			pd->length62 = *bp++;
			bp += formatIn(bp, bitmp62, FMT_MOVE ,8);
			memcpy(pd->bitmap, bitmp62, 8);
			pd->bitmp621 = bitmp62 [0];
			if (pd->bitmp621 & 0x80) {
				/* Authorization characteristics indicator */
				etoa(bp,1);
				bp += formatIn(bp, &msg->pos_pin_cap_code , FMT_CHAR,1); 
			}

			if (pd->bitmp621 & 0x40) {
				/* Transaction id */
				bp += formatIn(bp, &msg->storeid, FMT_BTOI, 7);
				bp += formatIn(bp, &msg->lane, FMT_BTOI, 8); 
			}

			if (pd->bitmp621 & 0x20) {
				/* Validation Code */
				etoa(bp,4);
				bp += formatIn(bp, pd->validation_code, FMT_CHAR,4); 
			}
			break;

		case 63:
			/* Special field. Implies debit processing if present */
			fieldlen = *bp++;		/*	length field */
			p = (char*) bp;		/*	bit map */
			bp += 3;

			memcpy(v_NetworkIdBitmap, p, 2);

			if (isBitSet(p, 0)) {
				bp += formatIn(bp, &msg->serial_1, FMT_BTOI, 4);
				msg->serial_1 = msg->serial_1 * 10000L; 
			}

			if (isBitSet(p, 1)) {
				bp += formatIn(bp, &msg->life_cycle, FMT_BTOI, 4);
				msg->life_cycle = msg->life_cycle * (60 * 60); 
			}

			if (isBitSet(p, 2))
				bp += formatIn(bp, &msg->revcode, FMT_BTOI, 4);

			if (isBitSet(p, 3))
				/* STIP Code */
				bp += formatIn(bp, &msg->revcode, FMT_BTOI, 4);

			if (isBitSet(p, 4))
				bp += formatIn(bp, &msg->serial_2, FMT_BTOI, 6);

			if (isBitSet(p, 5))
				/*	chargeback information */
				bp += 7;

			if (isBitSet(p, 6)) {
				fieldlen = *bp++;
				bp += 7;
				msg->serial_1 = msg->serial_1 + fieldlen; 
			}

			if (isBitSet(p, 7))
				/* business date */
				bp += 4;

			if (isBitSet(p, 8))
				bp += 12;

			break;

		case 64:
			/* MAC */
			externmsg->mac_found = 1;
			bp += formatIn(bp, msg->mac.key, FMT_BINARY, 8);
			break;


		case 66:
			/* settlement code */
			bp += formatIn(bp, &msg500->respcode, FMT_BTOI, 1);
			break;

		case 68:
			bp += 2;
			break;

		case 69:
			bp += 2;
			break;

		case 70:
			/* network management code */
			bp += formatIn(bp, &msg->netcode, FMT_BTOI, 3);
			break;

		case 73:
			bp += 3; /* date action */
			break;

		case 74:
			bp += formatIn(bp, &msg500->num_credit, FMT_BTOI, 10);
			break;

		case 75:
			bp += formatIn(bp, &msg500->num_credit_rev, FMT_BTOI, 10);
			break;

		case 76:
			bp += formatIn(bp, &msg500->num_debit, FMT_BTOI, 10);
			break;

		case 77:
			bp += formatIn(bp, &msg500->num_debit_rev, FMT_BTOI, 10);
			break;

		case 78:
			bp += formatIn(bp, &msg500->num_transfer, FMT_BTOI, 10);

		case 79:
			bp += formatIn(bp, &msg500->num_transfer_rev, FMT_BTOI, 10);
			break;

		case 80:
			bp += formatIn(bp, &msg500->num_inquiry, FMT_BTOI, 10);
			break;

		case 81:
			bp += formatIn(bp, &msg500->num_auth, FMT_BTOI, 10);
			break;

		case 82:
			bp += formatIn(bp, &msg500->fee_cr_processing, FMT_BTOM, 12);
			break;

		case 83:
			bp += formatIn(bp, &msg500->fee_cr_transaction,FMT_BTOM,12);
			break;

		case 84:
			bp += formatIn(bp, &msg500->fee_db_processing, FMT_BTOM, 12);
			break;

		case 85:
			bp += formatIn(bp,&msg500->fee_db_transaction, FMT_BTOM, 12);
			break;

		case 86:
			bp += formatIn(bp, &msg500->amt_credit, FMT_BTOM, 16);
			break;

		case 87:
			bp += formatIn(bp, &msg500->amt_credit_rev, FMT_BTOM, 16);
			break;

		case 88:
			bp += formatIn(bp, &msg500->amt_debit, FMT_BTOM, 16);
			break;

		case 89:
			bp += formatIn(bp, &msg500->amt_debit_rev, FMT_BTOM, 16);
			break;

		case 90:
			/* original data elements */
			/* Convert to ascii first */
			bp += formatIn(bp, &msg->origmsg, FMT_BTOI, 4);
			bp += formatIn(bp, &msg->origtrace, FMT_BTOI, 6);
			bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
			msg->origdate = auxdate + cardco_getcentury(cardco_local_currentdate());
			bp += formatIn(bp, &msg->origtime, FMT_BTOI, 6);
			bp += 11;  /* skip acq/forw. id's */
			break;

		case 91:
			++bp;
			break;

		case 92:
			bp += 2;
			break;

		case 93:
			/* always '00002' */
			bp += 5;
			break;

		case 95:
			etoa(bp, 12);
			bp += formatIn(bp, &msg->aval_balance, FMT_CTOM, 12);
			bp += (42 - 12);
			externmsg->replacement_amount = 1;
			break;

		case 97:
			/* amount net settlement */
			j = *bp++;
			bp += formatIn(bp, &msg500->amt_netset, FMT_CTOM, 16);
			if (j == 'D')
				decnmul(&msg500->amt_netset, (double)(-1));
			break;

		case 99:
			/* settlement agent */
			fieldlen = *bp++;
			bp += formatIn(bp, xbuf, FMT_BTOC, fieldlen);
			for(p = xbuf; *p == '0'; ++p);
			if (is500)
				strcpy(msg500->settlement_agent, p);
			break;

		case 100:
			/* forward institution */
			fieldlen = *bp++;
			bp += formatIn(bp, xbuf, FMT_BTOC, fieldlen);
			for(p = xbuf; *p == '0'; ++p) ;
			if (is500)
				strcpy(msg500->issuer, p);
			else
				strcpy(msg->issuer, p);
			break;

		case 101:
			/* file name */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			filename = (char*) bp;
			if (is300)
				formatIn(bp, msg300->file_name, FMT_CHAR, fieldlen);
			bp += fieldlen;
			break;

		case 102:
			/* check verification: 	account # 1 */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			bp += formatIn(bp, msg->acctnum, FMT_CHAR, fieldlen);
			break;

		case 103:
			fieldlen = *bp++;
			bp += fieldlen;
			break;

		case 120:
			fieldlen = *bp++;
			bp += 2;
			break;

		case 121:
			fieldlen = *bp++;
			bp += fieldlen;
			break;

		case 122:
			fieldlen = *bp++;
			bp += fieldlen;
			break;

		case 123:
			/* record data */
			/* AVS information */
			fieldlen = *bp++;
			etoa(bp, fieldlen);
			if (fieldlen >= sizeof(msg->addresponse)) {
				bp += fieldlen;
				logMsg("Rcvd additional response length (%d).\
				      This is too large.\n", fieldlen); }
			else
				bp += formatIn(bp, msg->addresponse, FMT_CHAR, fieldlen);
			break;

		case 127:
			if (is300 && isBitSet(bitmap, 1)) {
				++bp;
				/* It is an inquiry response only if
				   1. bit 127 is present; and
				   2. bit 2 (pan) is also present
				*/
				if (strcmp(msg300->file_name, "V.CH.EXP") == 0) {
					/*case SH_300_FILE_INQ:*/
					mupd = (struct cardco300fileupd *)msg300->action;

					etoa(bp, 1);
					bp += formatIn(bp, xbuf, FMT_CHAR, 1);
					mupd->updcode = xbuf[0];

					/* The pan */
					fieldlen = *bp++;
					bp += formatIn(bp, mupd->pan, FMT_BTOC, fieldlen);
					bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
					mupd->purge_date = auxdate + 20000000L;

					etoa(bp, 2);
					bp += formatIn(bp, mupd->action, FMT_CHAR, 2);

					etoa(bp, 9);
					bp += formatIn(bp, mupd->region, FMT_CHAR, 9);

					bp+=formatIn(bp,mupd->special_processing,FMT_BTOC,3);
				}
				else
					if (strcmp(msg300->file_name, "V.CH.PVV") == 0) {
						/*case	SH_300_PVV_INQ:*/
						mpvv = (struct cardco300pvv *)msg300->action;

						etoa(bp, 1);
						bp += formatIn(bp, xbuf, FMT_CHAR, 1);
						mpvv->updcode = xbuf[0];

						/* The pan */
						fieldlen = *bp++;
						bp += formatIn(bp, mpvv->pan, FMT_BTOC, fieldlen);

						bp += formatIn(bp, &auxdate, FMT_BTOI, 4);
						mpvv->purge_date = auxdate * 20000000L;

						bp += formatIn(bp, mpvv->algo, FMT_BTOC, 2);

						bp += formatIn(bp,mpvv->security_info,FMT_BTOC,6);
					}
			}
			else {
				p = (char*) bp;
				fieldlen = *bp++;
				bp += fieldlen;
			}

			break;

		case 128:
			break;

		}

		/*
		 ** Guarda tipo da mensagem e bitmap para 
		 ** uso em mensagens de saida roteadas
		 */
		sprintf(msg->filler1, "%04d", msg->msgtype);
		memcpy (msg->filler1 + 5, bitmap, 8);

		/*  Indicacao da mensagem vinda do VAP */
		memset(msg->filler3, '\0', sizeof(msg->filler3));
		msg->filler3[0] = 'V';

		/* =======================================================
		 * the formatIn and format_out() functions have internal
		 * debug calls. So turn off the fmt_debug before these
		 * function calls if you are recieving 800/810 messages .
		 * but now restore the old value
		 * 800/810 message
		 * ======================================================= */
		if (!extern800debug &&
		                (msgType == CARDCO_NETWORK ||
		                 msgType == CARDCO_NETWORK_RESPONSE) )
			fmt_debug = fmt_debugorg;  /* restore to original val */
	}

	return(0);
}
