#include <stdio.h>
#include <stdarg.h>
#include "cardco.h"
#include "extern.h"
#include "fila.h"

extern int achaEvento();

/* Verifica se txn de pagamento ou saldo */
isPayBalance(struct cardco *cardcoPtr)
{
        int ret;
        ret = 0;
        
        if (strncmp("SPF=", cardcoPtr->msg.acceptorname,4) == 0 ) 
        {    
          ret = 1;
        }
        else        
        if (atoi(cardcoPtr->msg.aval_balance) > 0)            
        {
          ret = 1;
        }        

        return ret;

}

unsigned char *extern_getbitmap(struct cardco *cardcomsg)
{
	/*
	 *	Find the bit map and map the message number
	 */
	static unsigned char *bitmap;
	int		type;

	/* Determine if msg is ATM or POS */
	if (cardcoIsSettlement(cardcomsg))
	    type = MSG_BOTH;
	else
	if (cardcoIs03xx(cardcomsg))
	    type = MSG_BOTH;
	else 
	if (cardcoIsATM(cardcomsg))
	    type = MSG_ATM;
	else
	    type = MSG_POS;

	bitmap = find_bitmap(cardcomsg->msg.msgtype, type);

	if(bitmap == NULL) return(NULL);

	return(bitmap);
}


set_pin_and_track(struct cardco *cardcomsg, struct cardcoexternmsg *vmsg)
{

	if(cardcomsg->msg.device_cap & CC_DEVCAP_NOTRACK_DATA)
	{
		if(cardcoIsFinancialRequest(cardcomsg))
		{
			logMsg("Error: A financial request does not contain track data.\n");
			return(-1);
		}
		
		/*	bit numbers are 1 less then actual bit numbers since
			setBitOff/setbiton() are zero based
		*/
		setBitOff(vmsg->bitmap, 34);  /* no track 2 */
		setBitOff(vmsg->bitmap, 44);  /* no track 1 */
		setBitOff(vmsg->bitmap, 51);  /* no pin     */
             /* Commenting out */
	     /* setBitOn(vmsg->bitmap, 13);	*//* expiry date */
	     /* End of commenting out. New code follows */
		setBitOff (vmsg->bitmap, 13); /* no pin     */
	}
	else if(cardcomsg->msg.device_cap & CC_DEVCAP_TRACK_1)
		setBitOn(vmsg->bitmap, 44);
	else
		setBitOn(vmsg->bitmap, 34);

     /* Commenting out */
     /* if(cardco_isbitset(vmsg->bitmap, 1))  */
     /*    if PAN is present then exp. date must also be present */
     /*    setBitOn(vmsg->bitmap, 13); */
     /*    Endof commenting out. New code follows */

	if (isBitSet(vmsg->bitmap, 1)) {
		char*	p;
		p = (char *) cardco_locate_track_data (cardcomsg);
		if (*p == (char) NULL)
			setBitOff (vmsg->bitmap, 13);
		else
			setBitOn  (vmsg->bitmap, 13);
	}

	if (cardcomsg->msg.pin[0]) {
	    /* PIN Is present */
	    setBitOn(vmsg->bitmap, 51);	/* pin present          */
	    setBitOn(vmsg->bitmap, 52);	/* security information */ }

	return(0);
}


adjust_bitmap(unsigned char *obp, unsigned char *ibp, ...)
{
	/*
	 *	This function takes an outgoing bitmap 'obp' and an original
	 *	bitmap 'ibp' and a list of bit positions (1 based) terminated by
	 *	'-1' and turns on all bits found in 'ibp' from the list in 'obp'
	 */
	va_list ap;
	int		b;

	va_start(ap, ibp);

	for(; (b = va_arg(ap, int)) > 0; )
	{
		--b;
		if(isBitSet(ibp, b))
			setBitOn(obp, b);
	}

	return(0);
}
printIMsg(struct internalmsg *msg)
{
        char    xbuf[64];

        printf("msgtype = %d\n", msg->msgtype);
        printf("pan=<%s>\n", msg->pan);
        printf("pcode = %d\n", msg->pcode);
        printf("issuer = <%s>\n", msg->issuer);
        printf("acquirer = <%s>\n", msg->acquirer);

        dectochar(&msg->settlement_amount, xbuf);
        printf("settlement_amount = <%s>\n", xbuf);

        fflush(stdout);

        dectochar(&msg->amount_equiv, xbuf);
        printf("amount_equiv = <%s>\n", xbuf);

        printf("trandate = <%d>\n", msg->trandate);

        dectochar(&msg->settlement_rate, xbuf);
        printf("settlement_rate = <%s>\n", xbuf);

        dectochar(&msg->iss_conv_rate, xbuf);
        printf("iss_conv_rate = <%s>\n", xbuf);

        printf("trace = %d\n", msg->trace);

        fflush(stdout);

        printf("local_time = %d\n", msg->local_time);

        printf("local_date = %d\n", msg->local_date);

        printf("Expiry date in Track-II=<%s>\n", msg->track2);

        printf("settlement_date = %d\n", msg->settlement_date);

        printf("iss_conv_date = %d\n", msg->iss_conv_date);

        printf("cap_date = %d\n", msg->cap_date);

        printf("merchant_type = <%d>\n", msg->merchant_type);

        printf("pos_entry_code = %d\n", msg->pos_entry_code);

        printf("network international identfifier = %d\n", msg->shift_number);

        fflush(stdout);

        printf("pos_condition_code = %d\n", msg->pos_condition_code);

        printf("pos_pin_cap_code = %d\n", msg->pos_pin_cap_code);

        dectochar(&msg->fee, xbuf);
        printf("fee=<%s>\n", xbuf);

        printf("acquirer = <%s>\n", msg->acquirer);

        printf("track2 = <%s>\n", msg->track2);

        fflush(stdout);

        printf("refnum = <%s>\n", msg->refnum);

        printf("authnum = <%s>\n", msg->authnum);

        printf("respcode = %d\n", msg->respcode);

        printf("Card Acceptor (termid)= <%s>\n", msg->termid);

        printf("Card Acceptor ID (termloc) = <%s>\n", msg->termloc);

        printf("acceptorname = <%s>\n", msg->acceptorname);

        fflush(stdout);

        printf("addresponse = <%s>\n", msg->addresponse);

        printf("acq_currency_code = %d\n", msg->acq_currency_code);

        printf("settlement_code = %d\n", msg->settlement_code);

        printf("iss_currency_code = %d\n", msg->iss_currency_code);

        fflush(stdout);

        return(1);
}


get_original_message(struct cardco *cardcomsg, struct cardco *omsg)
{
	if(cardco_locate_event(cardcomsg, omsg) < 0)
	{
		cardcomsg->msg.msgtype = CARDCO_FINREQ_RESPONSE;
		if(cardco_locate_event(cardcomsg, omsg) < 0)
			logMsg("Unable to retrevie original transaction for %ld\n",
			cardcomsg->msg.trace);
	}

}

int cardco_locate_event(struct cardco *cardcomsg, struct cardco *omsg)
{
		struct cardco * ccptr;
		achaEvento(cardcomsg->msg.trace, cardcomsg->msg.trantime, 0, omsg);
	
}

do_file_update(struct cardco *cardcomsg, struct cardcoexternmsg *vmsg,
												char *externbuf, int len)
{
	/*
	 *	A extern/host file update has occured. It is our
	 *	job to pass this on to the proper end-point
	 *	For now, this release, there is no editing performed
	 *	on the transaction.
	 */

	int		id;			/*	mail box id */
	static	struct	cardcopkg	*MIPpkg = NULL;
	static	int	uf		 = 0;
	int		i;
	struct	save302	save302;

	/* Find the mail box where we can send stuff to the mip (ie. EXT) */
	/*
	if(MIPpkg == NULL)
	{

		if(cardco_locate_bin(&MIPpkg, MIPid) < 0)
		{
			logMsg("Unable to locate the MIP Mailbox (id: %s)\n", MIPid);
			logMsg("%ld message ignored.\n", cardcomsg->msg.msgtype);
			return(-1);
		}
	}
	*/

	/* determine output mailbox */
	if(cardcomsg->msg.msgtype == 302)
	{
		/* determine the MIP mail box */
/*		id = MIPpkg->bin.creditport; */

		/* generate a unique user info value */
		uf = (uf + 1) & 0xff;

		/* save the required information before we create an event */
		save302.origmbid	= 5;
		save302.origuf		= vmsg->header->user_info;
		save302.uf			= uf;

		/* create an event so we may determine routing later */
/*		i = (int) tm_enqueue(mbid_in, time((long *)0) + 100, TM_NOTIFY_ONCE,
			(int) ((302 * 1000) + uf), (char*) &save302, sizeof(save302)); */
		
		/* set the no expire flag */
/*		tm_setflags(i, TM_NOSAVE | TM_NOEXPIRE, 1); */

		/* update the outgoing EXT header to include our user info */
		vmsg->header->user_info = uf;
	}
	else
	{
		/* This is a little more difficult: We must determine the
		   issuer from the saved transaction.
		   This request (the 312) has come back from EXT
		*/

		/* rebuild our event key */
		i = (302 * 1000) + vmsg->header->user_info;

		/*	Locate the event */
/*		i = tm_locate(i, -1); */
		if(i < 0)
		{
			logMsg("Unable to recover 302 event with ID (302%03d)\n",
				vmsg->header->user_info);
			return(-1);
		}

		/* Read the saved data and restore the message */
/*		tm_getdata(i, (char*) &save302, sizeof(save302)); */

		/* Remove the event */
/*	tm_dequeue(i); */

		/* now restore the needed values */
		vmsg->header->user_info = save302.origuf;

		/* determine where to send the message */
		id = save302.origmbid;
	}


	/* Now send the 302/312 message to its proper destination */
	gravaFila(FUPDQ, externbuf, len);
}
