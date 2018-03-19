/*#ident "@(#) cardco:v_in.c 1.00 19Mar05"*/
/*
 *	Visa handler de entrada. Mensagens vindas da proceda sao processadas
 *  e roteadas para o autorizador.
 *
 *
 *      Modification Hcardcoory
 *
 *	Who		When		Why
 *	========================================================================
 *
 *
 *
 *
 */

#include <time.h>
#include "cardco.h"
#include "extern.h"
#include "fila.h"
static	int	trace = 0;

extern_inbound()
{
	/*
	 *	Read messages from the comm line and give to the switch
	 */
	int 		len, i;
	int			is300;
	int			fmt_ok;
	int			last_sender;
	int			auxportid;
	char		auxname[50];
	unsigned	char *pvb;
	struct		private_data	*pd;

	logMsgSetPrefix("FromVisa");

	for(;;)
	{
		if((len = leFila(FORMINQ, externbuf, sizeof(externbuf))) < 0)
		{
			logMsg("%s:leFila(): %d\n",__FILE__, len);
			break;
		}
		dumpMsg(externbuf, len, "FromVisa");
		/* Quatro bytes contem o tamanho */
		pvb = externbuf + 4;


		/* Determine type of header and get proper pointers */
		if(get_header(pvb, &externmsg) < 0)
			continue;


		/*
		*  If 800 type message and user don't want 800 message to be
		*     put to debug file. Skip Debug.
		*  else
		*     put debug message into file.
		*/
		msgType = btoi(externmsg.msgno, 4);
		if((extern800debug) ||
		                (msgType != 800 && msgType != 810))
		{
			int	i;

			debug("******************  In Bound Message **************\n");
			for(i = 0; i < len; ++i)
				debug("%02x", pvb[i]);
			debug("\n");

			debug("Bits found:[");

			for(i = 0; i < 128; ++i)
				if ((isBitSet(externmsg.bitmap, i)) &&
				                ((i <= 64) || (isBitSet(externmsg.bitmap, 1))))
					/*
					if(cardco_isbitset(externmsg.bitmap, i))
					*/
					debug("%d,", i );
			debug("]\n");

			/* extern_print_header(externmsg.header);*/
		}
		/*
		 *  Decode the message
		 */
		memset(&cardcomsg, 0, sizeof(cardcomsg));
		/* cardco_get_current_date(); */
		fmt_ok = externToCardCo(&cardcomsg, &externmsg);

		if(cardcoIs03xx(&cardcomsg))
			is300 = 1;
		else
			is300 = 0;

		if(!is300)
			logMsg("m%04d/a%010s/t%06ld/p%06ld/r%02ld\n",
			      cardcomsg.msg.msgtype, cardcomsg.msg.acquirer,
			      cardcomsg.msg.trace,cardcomsg.msg.pcode,cardcomsg.msg.respcode);
		else
			logMsg("m%04d/p%06ld\n",
			      cardcomsg.msg.msgtype, cardcomsg.msg.pcode);

		/* added code bellow to ignore the message in case of error */
		if (fmt_ok < 0) continue;

		/* If file update message: We have already sent it */
		if(cardcomsg.msg.msgtype == 302 || cardcomsg.msg.msgtype == 312)
		{
			if(extern_support300 == 0)
			{
				/* support old style processing */
				do_file_update(&cardcomsg, &externmsg, pvb, len);
				continue;
			}
		}


		if(cardcomsg.msg.msgtype == 110)
		{
			if(cardcoIsATM(&cardcomsg))
				cardcomsg.msg.msgtype = 210;
		}

		else if(cardcomsg.msg.msgtype == 130)
		{
			if(cardcoIsATM(&cardcomsg))
				cardcomsg.msg.msgtype = 230;
		}

		/* Adjust values */
		if(extern_to_cardco(&cardcomsg, &externmsg) < 0)
			/*	Throw away the message */
			continue;

		/* Drop message if advice not approved */
		/* after patch of cardco_edit.c we can take this off
		if ( (cardcoIsAdvice (&cardcomsg) ) && (cardcomsg.msg.respcode != 0) )
			continue;
		*/

		/* Save my message header */
		extern_save_information(&cardcomsg, &externmsg);

		printIMsg(&cardcomsg.msg);

		/* Send the message */
		if(cardcoIsNetwork(&cardcomsg) && !cardcoIsResponse(&cardcomsg)
		                && cardcomsg.msg.netcode == CARDCO_NET_ECHO)
		{
			/* Mensagem de eco. Devolve para a rede */
			cardco_set_return_msgno(&cardcomsg);
			cardcomsg.msg.respcode = 0;
			gravaFila(DRIVERQ,&cardcomsg.msg, sizeof(cardcomsg.msg));
		}
		else {
			if(isPayBalance(&cardcomsg)) {
				/* Manda para o autorizador */
				if(gravaFila(PAYBAL,(char *) &cardcomsg.msg,sizeof(struct internalmsg)) < 0)
				{
					logMsg("gravaFila(%d,  buf, size): Error\n",PAYBAL);
					break;
				}
			}
			else {
				/* Manda para o autorizador */
				if(gravaFila(AUTHQ,(char *) &cardcomsg.msg,sizeof(struct internalmsg)) < 0)
				{
					logMsg("gravaFila(%d,  buf, size): Error\n",AUTHQ);
					break;
				}
			}
		}
	}
}


extern_to_cardco(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	/*
	 *	Make adjustments to the extern message
	 */
	int		i;

	if(!cardcoIs03xx(cardcoPtr))
	{
		/* Map message number */
		extern_make_cardcomsgno(cardcoPtr);

		/* Determine transaction type from processing code */
		/* commented out bellow : already done
		  if(extern_make_cardcopcode(cardcoPtr) < 0)
		     return(-1);
		 */

		if(externmsg->isreject)
			return(extern_cardcoreject(cardcoPtr, externmsg));
	}


	/* Map response and revreason codes */
	switch(cardcoPtr->msg.msgtype){
	case	CARDCO_AUTHREQ:
	case	CARDCO_AUTHREQ_REPEAT:
	case	CARDCO_AUTHREQ_ADVICE:
	case	CARDCO_FINREQ:
	case	CARDCO_FINREQ_REPEAT:
	case	CARDCO_FINREQ_ADVICE:
		extern_cardcoauthreq(cardcoPtr, externmsg);
		break;

	case	CARDCO_AUTHREQ_RESPONSE:
	case	CARDCO_AUTHREQ_ADVICE_RESPONSE:
	case	CARDCO_FINREQ_RESPONSE:
	case	CARDCO_FINREQ_ADVICE_RESPONSE:
		extern_cardcoauthresp(cardcoPtr,externmsg);
		extern_locate_originator(cardcoPtr);
		break;

	case	CARDCO_REVREQ:
	case	CARDCO_REVREQ_REPEAT:
	case	CARDCO_REVREQ_ADVICE:
	case	CARDCO_AUTHCOMP:/*	actualy a reversal */
		extern_cardcorevreq(cardcoPtr, externmsg);
		break;

	case	CARDCO_REVREQ_RESPONSE:
		extern_cardcorevresp(cardcoPtr,externmsg);
		extern_locate_originator(cardcoPtr);
		break;

	case	CARDCO_NETWORK:
	case	CARDCO_NETWORK_RESPONSE:
		extern_cardconetwork(cardcoPtr,externmsg);
		extern_locate_originator(cardcoPtr);
		break;

	case	CARDCO_FILEUPD_ISSREQ:
	case	CARDCO_FILEUPD_ISSREQ_RESPONSE:
		/* File update stuff: We just pass this on to Ext */
		extern_300_process(cardcoPtr, externmsg);
		break;

	default:
		break;
	}


	// if(!cardcoIs03xx(cardcoPtr) && extern_to_cardco_poscodes(cardcoPtr) < 0) return(-1);
	/* Now we should do some sanity checks here */
	return(0);
}


extern_to_cardco_poscodes(struct cardco *cardcoPtr)
{
	int i;

	/* Set the conditon codes and other stuff */
	switch(cardcoPtr->msg.pos_entry_code / 10){
	case	0:	i = CC_POS_PANENTRY_UNKNOWN;	break;
	case	1:	i = CC_POS_PANENTRY_MANUAL;		break;
	case	2:	i = CC_POS_PANENTRY_MAGSTRIP;	break;
	case	3:	i = CC_POS_PANENTRY_BARCODE;	break;
	case	4:	i = CC_POS_PANENTRY_OCR;		break;
	case	5:	i = CC_POS_PANENTRY_IC;			break;
	case	90:	i = CC_POS_PANENTRY_TRK1OR2;	break;
	default:
		logMsg("Unknown 'terminal entry mode' '%02d'\n",
		      cardcoPtr->msg.pos_entry_code / 10);
		return(-1);
	}

	i = i * 10;

	switch(cardcoPtr->msg.pos_entry_code % 10){
	case	0:	i +=	CC_POS_PINENTRY_UNKNOWN;	break;
	case	1:	i +=	CC_POS_PINENTRY_HASCAP;		break;
	case	2:	i +=	CC_POS_PINENTRY_NOCAP;		break;
	case	8:	i += 	CC_POS_PINENTRY_INOPERATIVE;	break;
	case	9:	i +=	CC_POS_PINENTRY_VERIFIED;	break;
	default:
		logMsg("Invalid pin entry mode: '%d'\n",cardcoPtr->msg.pos_entry_code % 10);
		return(-1);
	}

	cardcoPtr->msg.pos_entry_code = i;

	switch(cardcoPtr->msg.pos_condition_code){
	case	0:
		/* normal transaction of this type
		   I don't have same information as with Master card so I
		   fudge some of these values
		*/
		cardcoPtr->msg.pos_condition_code =
		        CC_POS_COND_CUSTPRESENT | CC_POS_COND_CARDPRESENT
		        | CC_POS_COND_REQ_NORMAL;
		break;

	case	1:
		/* Customer not present */
		cardcoPtr->msg.pos_condition_code = 0;
		break;

	case	2:
		cardcoPtr->msg.pos_condition_code =
		        CC_POS_COND_CUSTPRESENT | CC_POS_COND_CARDPRESENT
		        | CC_POS_COND_REQ_NORMAL;
		break;

	case	3:
		/* merchant suspicious */
		cardcoPtr->msg.pos_condition_code = CC_POS_COND_SUSPECT;
		break;

	case	5:
		cardcoPtr->msg.pos_condition_code = CC_POS_COND_CUSTPRESENT;
		break;

	case	8:
		/* mail order */
		cardcoPtr->msg.pos_condition_code = CC_POS_COND_REQ_MAIL;
		break;

	case	10:
		/* customer idenity verified */
		cardcoPtr->msg.pos_condition_code = CC_POS_COND_IDENTITY_VER;
		break;

	case	51:
		/* verification request */
		cardcoPtr->msg.pos_condition_code = CC_POS_COND_AVS;
		break;

	default:
		logMsg("Invalid pos condition code: '%02d'\n",
		      cardcoPtr->msg.pos_condition_code);
		cardcoPtr->msg.pos_condition_code = 0;
		break;
	}



	switch(cardcoPtr->msg.pos_cap_code / 10){
	case	0: cardcoPtr->msg.pos_cap_code = CC_POS_CAP_UNSPECIFIED;
		break;
	case	2: cardcoPtr->msg.pos_cap_code = CC_POS_CAP_UNATTENDED;
		break;
	case	4: cardcoPtr->msg.pos_cap_code = CC_POS_CAP_ECR;
		break;
	case	7: cardcoPtr->msg.pos_cap_code = CC_POS_CAP_DIALTERM;
		break;
	default:
		logMsg("Invalid device type: '%d'\n", cardcoPtr->msg.pos_cap_code / 10);
		break;
	}

	switch(cardcoPtr->msg.pos_cap_code % 10){
	case	0:
	case	1:
	case	2:
	case	3:
	case	4:
	case	5:
	case	6: break;
	case	9: cardcoPtr->msg.pos_cap_code |= CC_POS_CAP_NOCARDREAD;	break;
	default:
		logMsg("Invalid term cap. code: '%d'\n",
		      cardcoPtr->msg.pos_cap_code % 10);
		break;
	}

	return(0);
}


extern_make_cardcopcode(struct cardco *cardcoPtr)
{
	int		txn, from_acct, to_acct;

	/*
	 *	format the transaction type and the account types
	 */

	txn = cardcoPtr->msg.pcode / 10000;
	from_acct	= cardcoPtr->msg.pcode / 100 % 100;
	/* make sure to_acct is zero. Not zero means corrupted message */
	to_acct = 0;  /* not used in extern */
	to_acct	= cardcoPtr->msg.pcode % 100;

	if(txn == 0)
		/* goods or services purchase */
		cardcoPtr->msg.txntype = CC_TX_SERVICES;
	else if(txn == 1)
		cardcoPtr->msg.txntype = CC_TX_WD;
	else if(txn == 3)
		cardcoPtr->msg.txntype = CC_TX_CHECK_GUARANTEE;
	else if(txn == 30 || txn >= 90)
		cardcoPtr->msg.txntype = CC_TX_BAL;
	else if(txn == 20)
		cardcoPtr->msg.txntype = CC_TX_DEP;
	else if(txn == 28)
		cardcoPtr->msg.txntype = CC_TX_DEP;
	else if(txn == 40)
		cardcoPtr->msg.txntype = CC_TX_TSF;
	else if(txn == 84 || txn == 82)
		cardcoPtr->msg.txntype = CC_TX_BILLPAY;
	else
	{
		/* error */
		cardcoPtr->msg.txntype = 0;
		logMsg("Unable to determine txn type: %d\n", txn);
		return(-1);
	}

	if(from_acct == 10)
		cardcoPtr->msg.from_acct = CC_ACCT_SAV;
	else if(from_acct == 20)
		cardcoPtr->msg.from_acct = CC_ACCT_CHEQ;
	else if(from_acct == 30)
		cardcoPtr->msg.from_acct = CC_ACCT_CC;
	else if(from_acct == 0)
		cardcoPtr->msg.from_acct = CC_ACCT_CC_0;
	else
	{

		logMsg("Unsupported account type: %d\n", from_acct);
		return(-1);

	}
	/* make sure to_acct is zero. Not zero means corrupted message */
	if (to_acct) return (-1);
	to_acct	= 0; /* not used in extern */

	return(0);
}


extern_make_cardcomsgno(struct cardco *cardcoPtr)
{
	switch(cardcoPtr->msg.msgtype){
	case	100:	cardcoPtr->msg.msgtype = CARDCO_AUTHREQ;
		break;
	case	101:	cardcoPtr->msg.msgtype = CARDCO_AUTHREQ_REPEAT;
		break;
	case	102:	cardcoPtr->msg.msgtype = CARDCO_AUTHCOMP;
		break;
	case	120:	cardcoPtr->msg.msgtype = CARDCO_AUTHREQ_ADVICE;
		break;
	case	130:	cardcoPtr->msg.msgtype = CARDCO_AUTHREQ_ADVICE_RESPONSE;
		break;
	case	110:	cardcoPtr->msg.msgtype = CARDCO_AUTHREQ_RESPONSE;
		break;
	case	200:	cardcoPtr->msg.msgtype = CARDCO_FINREQ;
		break;
	case	201:	cardcoPtr->msg.msgtype = CARDCO_FINREQ_REPEAT;
		break;
	case	220:	cardcoPtr->msg.msgtype = CARDCO_FINREQ_ADVICE;
		break;
	case	230:	cardcoPtr->msg.msgtype = CARDCO_FINREQ_ADVICE_RESPONSE;
		break;
	case	210:	cardcoPtr->msg.msgtype = CARDCO_FINREQ_RESPONSE;
		break;
	case	400:	cardcoPtr->msg.msgtype = CARDCO_REVREQ;
		break;
	case	401:	cardcoPtr->msg.msgtype = CARDCO_REVREQ_REPEAT;
		break;
	case	420:	cardcoPtr->msg.msgtype = CARDCO_REVREQ_ADVICE;
		break;
	case	430:	cardcoPtr->msg.msgtype = CARDCO_REVREQ_ADVICE_RESPONSE;
		break;
	case	410:	cardcoPtr->msg.msgtype = CARDCO_REVREQ_RESPONSE;
		break;
	case	800:	cardcoPtr->msg.msgtype = CARDCO_NETWORK;
		break;
	case	810:	cardcoPtr->msg.msgtype = CARDCO_NETWORK_RESPONSE;
		break;
	default:
		logMsg("Invalid message number: %d\n", cardcoPtr->msg.msgtype);
		break;
	}
}



extern_cardcoauthreq(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	/*
	 *	Format an inbound authorization request
	 */

	long   timenow;
	struct tm  *tm;
	
	if(!isBitSet(externmsg->bitmap, 10))
	{
		/* No trace number we should create our own */
		/* cardcoPtr->msg.trace = cardco_gmt_date.tm_yday + 1 + trace * 1000; */
		/* a variavel cardco_gmt_date eh gerada para SCO */
		time(&timenow);
		tm = localtime(&timenow);
		cardcoPtr->msg.trace = tm->tm_yday + 1 + trace * 1000;
		trace = (trace + 1) % 1000;
	}

	if(!isBitSet(externmsg->bitmap, 11))
	{
		/* date/time local */
		cardcoPtr->msg.local_date = cardco_local_currentdate();
		cardcoPtr->msg.local_time = cardco_local_currenttime();
	}

}


extern_cardcoauthresp(struct cardco *cardcoPtr, struct cardcoexternmsg *vmsg)
{
	/*
	 *	Format response code
	 */
	struct	cardco		omsg;
	char	tb[20];

	extern_cardcorespcode(cardcoPtr);

	/*If local date/time not present we retrieve from original transaction*/
	if(cardcoPtr->msg.trace <= 0)
	{
		memcpy(tb, &cardcoPtr->msg.refnum[6], 6);
		tb[6] = 0;
		cardcoPtr->msg.trace = atoi(tb);
		logMsg("Refnum:%s Trace:%d\n", cardcoPtr->msg.refnum, cardcoPtr->msg.trace);
	}

	get_original_message(cardcoPtr, &omsg);
	cardcoPtr->msg.local_date = omsg.msg.local_date;
	cardcoPtr->msg.local_time = omsg.msg.local_time;
	cardcoPtr->msg.settlement_date = omsg.msg.settlement_date;
	cardcoPtr->msg.trace = omsg.msg.trace;
	strcpy(cardcoPtr->msg.acquirer, omsg.msg.acquirer);
	strcpy(cardcoPtr->msg.originator, omsg.msg.originator);
	strcpy(cardcoPtr->msg.track2, omsg.msg.track2);
	cardcoPtr->msg.merchant_type = omsg.msg.merchant_type;
	cardcoPtr->msg.origmsg		= omsg.msg.origmsg;
	cardcoPtr->msg.origtrace		= omsg.msg.origtrace;
	cardcoPtr->msg.origdate		= omsg.msg.origdate;
	cardcoPtr->msg.origtime		= omsg.msg.origtime;
	cardcoPtr->msg.pos_entry_code	= omsg.msg.pos_entry_code;

}


extern_cardcorevreq(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	/*
	 *	format reversal request
	 */

	/* There is no response code in EXT reversal */
	if(cardcoPtr->msg.msgtype == CARDCO_REVREQ ||
	                cardcoPtr->msg.msgtype == CARDCO_REVREQ_REPEAT)
		cardcoPtr->msg.respcode = CARDCO_RC_UnableToProcess;

	if(externmsg->replacement_amount)
	{
		cardcoPtr->msg.revcode = CARDCO_RR_PartialDispense;
		cardcoPtr->msg.respcode = CARDCO_RC_PartialDispense;
	}
	else
		cardcoPtr->msg.revcode = CARDCO_RR_CancelRequest;
}

extern_cardcorevresp(struct cardco *cardcoPtr, struct cardcoexternmsg *vmsg)
{
	/*
	 *	Reversal request response
	 */
	struct cardco omsg;
	char	tb[30];

	cardcoPtr->msg.respcode = 0;

	if(cardcoPtr->msg.trace <= 0)
	{
		memcpy(tb, &cardcoPtr->msg.refnum[6], 6);
		tb[6] = 0;
		cardcoPtr->msg.trace = atoi(tb);
		logMsg("Refnum:%s Trace:%d\n", cardcoPtr->msg.refnum, cardcoPtr->msg.trace);
	}

	get_original_message(cardcoPtr, &omsg);
	cardcoPtr->msg.local_date 	= omsg.msg.local_date;
	cardcoPtr->msg.local_time	= omsg.msg.local_time;
	cardcoPtr->msg.settlement_date 	= omsg.msg.settlement_date;
	cardcoPtr->msg.trace 		= omsg.msg.trace;
	strcpy(cardcoPtr->msg.acquirer, omsg.msg.acquirer);
	strcpy(cardcoPtr->msg.originator, omsg.msg.originator);
	strcpy(cardcoPtr->msg.track2, omsg.msg.track2);
	cardcoPtr->msg.merchant_type 	= omsg.msg.merchant_type;
	cardcoPtr->msg.origmsg		= omsg.msg.origmsg;
	cardcoPtr->msg.origtrace	= omsg.msg.origtrace;
	cardcoPtr->msg.origdate		= omsg.msg.origdate;
	cardcoPtr->msg.origtime		= omsg.msg.origtime;

}

extern_cardconetwork(struct cardco *cardcoPtr, struct cardcoexternmsg *vmsg)
{
	/*
	 *	Network message
	 */

	bin_t	bin;

	/* Map the netcode */
	switch(cardcoPtr->msg.netcode){
	case	1: cardcoPtr->msg.netcode = CARDCO_NET_SIGNON;		break;
	case	2: cardcoPtr->msg.netcode = CARDCO_NET_SIGNOFF;		break;
	case	0:
	case  301: cardcoPtr->msg.netcode = CARDCO_NET_ECHO;		break;
	case   62: cardcoPtr->msg.netcode = CARDCO_NET_ENTER_SI;	break;
	case   63: cardcoPtr->msg.netcode = CARDCO_NET_EXIT_SI;		break;
	default:   							break;
	}

	/* set the proper station id */
	if(cardcoIsResponse(cardcoPtr))
	{
		btos(vmsg->header->destination, bin, 6);
		if(cardcoPtr->msg.acquirer[0] == '\0')
			strcpy(cardcoPtr->msg.acquirer, bin);
		if(cardcoPtr->msg.issuer[0] == '\0')
			strcpy(cardcoPtr->msg.issuer, bin);
	}
	else
	{
		strcpy(cardcoPtr->msg.acquirer, MIPid);
		btos(vmsg->header->destination, bin, 6);
		strcpy(cardcoPtr->msg.issuer, bin);
		strcpy(cardcoPtr->msg.originator, cardco_orgid);
	}

}


extern_cardcorespcode(struct cardco *cardcoPtr)
{
	int	resp;

	if(cardcoIs03xx(cardcoPtr))
		resp = ((struct cardco300 *)&cardcoPtr->msg)->respcode;
	else
		resp = cardcoPtr->msg.respcode;

	switch(resp){
	case	0: resp = CARDCO_RC_Approved;
		break;
	case	1:
	case	2: resp = CARDCO_RC_ReferCardIssuer;
		break;
	case	3: resp = CARDCO_RC_InvalidMerchant;
		break;
	case	4: resp = CARDCO_RC_HotCardPickUp;
		break;
	case	5: resp = CARDCO_RC_DoNotHonor;
		break;
	case	6: resp = CARDCO_RC_UnableToProcess;
		break;
	case	7: resp = CARDCO_RC_HotCardPickUp;
		break;
	case   11: resp = CARDCO_RC_Approved;
		break;
	case   12: resp = CARDCO_RC_InvalidTransaction;
		break;
	case   13: resp = CARDCO_RC_InvalidAmount;
		break;
	case   14: resp = CARDCO_RC_InvalidCard;
		break;
	case   15: resp = CARDCO_RC_InvalidIssuer;
		break;
	case   19: resp = CARDCO_RC_SystemErrorReenter;
		break;
	case   21: resp = CARDCO_RC_UnableToProcess;
		break;
		/*
			case   25: resp = CARDCO_RC_RecordNotFound;
				   break;
			case   28: resp = CARDCO_RC_FileNotAvailable;
				   break;
		*/
	case   41: resp = CARDCO_RC_HotCard;
		break;
	case   43: resp = CARDCO_RC_HotCardPickUp;
		break;
	case   51: resp = CARDCO_RC_NoFunds;
		break;
	case   52: resp = CARDCO_RC_InvalidAccount;
		break;
	case   53: resp = CARDCO_RC_InvalidAccount;
		break;
	case   54: resp = CARDCO_RC_ExpiredCard;
		break;
	case   55: resp = CARDCO_RC_IncorrectPIN;
		break;
	case   57: resp = CARDCO_RC_InvalidTransaction;
		break;
	case   61: resp = CARDCO_RC_ExceedsLimit;
		break;
	case   62: resp = CARDCO_RC_RestrictedCard;
		break;
	case   63: resp = CARDCO_RC_MACKeyError;
		break;
	case   75: resp = CARDCO_RC_ExceedsPINRetry;
		break;
	case   76: resp = CARDCO_RC_UnableToProcess;
		break;
	case   77: resp = CARDCO_RC_UnableToProcess;
		break;
	case   80: resp = CARDCO_RC_InvalidCaptureDate;
		break;
	case   82: resp = CARDCO_RC_ExceedsFreqLimit;
		break;
	case   83: resp = CARDCO_RC_UnableToProcess;
		break;
	case   85: resp = CARDCO_RC_UnableToProcess;
		break;
	case   86: resp = CARDCO_RC_IncorrectPIN;
		break;
	case   91: resp = CARDCO_RC_SwitchNotAvailable;
		break;
	case   96: resp = CARDCO_RC_SystemError;
		break;
	default:   resp = CARDCO_RC_UnableToProcess;
		break;
	}

	if(cardcoIs03xx(cardcoPtr))
		((struct cardco300 *)&cardcoPtr->msg)->respcode = resp;
	else
		cardcoPtr->msg.respcode = resp;

	return(resp);
}


extern_locate_originator(struct cardco *cardcoPtr)
{
	/*
	 *	Determine who the originator of the message is
	 *
	 *	Currently the originator is always the switch (never call this
	 *	function for a request; only on a reply)
	 *
	 *	This stratagy works only if we always talk to a host.
	 *	If we are between two networks then a more work (at the switch)
	 *	needs to be done
	 */

	if(cardcoPtr->msg.msgtype == CARDCO_NETWORK)
		return(0);

	strcpy(cardcoPtr->msg.originator, cardco_orgid);
	return(0);
}




extern_cardcoreject(struct cardco *cardcoPtr, struct cardcoexternmsg *externmsg)
{
	/*
	 *	Ext reject message:	We must treat it as a reversal
	 */
	switch(cardcoPtr->msg.msgtype){
	case	CARDCO_AUTHREQ:
	case	CARDCO_AUTHREQ_REPEAT:
	case	CARDCO_AUTHREQ_ADVICE:
		/* reject message on a request
		 * We must treat it as though the issuer rejected
		 */

		/* Convert to response and supply an error code */
		cardcoPtr->msg.msgtype = CARDCO_AUTHREQ_RESPONSE;
		cardcoPtr->msg.respcode = CARDCO_RC_RejectMessage;
		break;

	case	CARDCO_AUTHREQ_RESPONSE:
		/*
		 * I have sent a response but he was unable to process it
		 * We must assume then that the transaction was reversed on the
		 * other side and so we must do the same here
		 */

		/* Convert the message number and supply the error code */
		cardcoPtr->msg.msgtype = CARDCO_REVREQ;
		cardcoPtr->msg.revcode = CARDCO_RR_RejectMessage;
		cardcoPtr->msg.respcode = CARDCO_RC_RejectMessage;
		break;

	case	CARDCO_REVREQ:
		/*
		 * I have attempted to revers a transaction
		 * Ext did not like the message
		 * The resolution is to convert this into a reversal response
		 * The reasoning behind this is that the switch has already
		 * processed the reversal and if EXT did not then it becomes
		 * a settlement issue.
		 */
		cardcoPtr->msg.msgtype = CARDCO_REVREQ_RESPONSE;
		break;

	case	CARDCO_REVREQ_RESPONSE:
		/*
		 * I have been sent a 400 request: The response did not work.
		 * The resolution is to ignore this message:
		 */
		return(-1);

	default:
		return(-1);
	}

	return(0);
}


extern_300_process(struct cardco *cardcoPtr, struct cardcoexternmsg *vmsg)
{
	/*
	 *	Format response code
	 */
	struct	cardco		omsg;
	struct	cardco300	*m300, *o300;

	m300 = (struct cardco300 *)&cardcoPtr->msg;
	o300 = (struct cardco300 *)&omsg.msg;
	if(m300->msgtype == 302)
		/* nothing to do: currently unsupported */
		return(0);

	extern_cardcorespcode(cardcoPtr);

	/*If local date/time not present we retrieve from original transaction*/
/*
	if(cardco_locate_event_300(cardcoPtr, &omsg) < 0)
	{
		logMsg("Unable to re-locate %d request. Trace %ld\n",
		      m300->msgtype, m300->trace);
		return(-1);
	}
*/
	m300->pcode = o300->pcode;
	strcpy(m300->acquirer, o300->acquirer);
	strcpy(m300->issuer, o300->issuer);

	return(0);
}
