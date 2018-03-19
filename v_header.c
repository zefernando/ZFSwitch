/*#ident "@(#) cardco:v_header.c 1.00 18Jun97"*/
/*
 *
 *	
 *      Functions to manipulate the header	
 *
 *
 *
 *	Modification History
 *
 *	Who		When		Why
 *	========================================================================
 *
 */

#include <stdio.h>
#include "cardco.h"
#include "extern.h"


get_header(unsigned char *msg, struct cardcoexternmsg *vmsg)
{
	/*
	 *	Determine where the bit map is located in the message
	 *	Find the data, find the header
	 */
	register struct	cardcoexternheader *hp;

	memset(vmsg, 0, sizeof(struct cardcoexternmsg));
	hp = (struct cardcoexternheader *)msg;

	/* Determine start of text */
	vmsg->header = (struct cardcoexternheader *)msg;
	vmsg->bitmap = msg + hp->len + EXT_MESSAGE_NUMBER_LEN;
	vmsg->msgno  = msg + hp->len;

	/* Determine if this is a reject message or not */
	if(hp->len >= EXT_HEADER_LEN &&
	                (((struct cardcoexternheader_reject *)hp)->bitmap[0] & V_BIT1))
		vmsg->isreject = 1;

	/* Determine how many bitmaps exist */
	if(isBitSet(vmsg->bitmap, 0))
		vmsg->msg = vmsg->bitmap + 2 * EXT_BITMAP_LEN;
	else
		vmsg->msg = vmsg->bitmap + EXT_BITMAP_LEN;
	

	extern_print_header(hp);
	return(0);
}


build_header(struct cardco *cardcomsg, unsigned char *msg,
             register struct cardcoexternmsg *vmsg)
{
	/*
	 *	Given	a extern message number in cardcomsg
	 *			a bitmap in vmsg->bitmap
	 *	This function constructs the header and
	 *	determines where the data pointers within
	        *	*msg are located
	 */
	int	maplen;
	register struct private_data *hp;

	/* Build new message header */
	if(isBitSet(vmsg->bitmap, 0))
		maplen = 16;
	else
		maplen = 8;

	vmsg->header = (struct cardcoexternheader *)msg;
	msg += sizeof(struct cardcoexternheader);
	vmsg->msgno = msg;
	msg += EXT_MESSAGE_NUMBER_LEN;
	memcpy(msg, vmsg->bitmap, maplen);
	vmsg->bitmap = msg;
	msg += maplen;
	vmsg->msg = msg;


	vmsg->header->len = sizeof(struct cardcoexternheader);
	vmsg->header->header_format = 1;
	memcpy(vmsg->header->source, source_station_id, 3);
	vmsg->header->text_format  = 1;
	if(cardcoIsResponse(cardcomsg))
	{
		/* re-use some of the old values */
		if(cardcoIs03xx(cardcomsg))
			hp = (struct private_data *)
			     ((struct cardco300 *)&cardcomsg->msg)->formatter_use;
		else
			hp = (struct private_data *)cardcomsg->msg.formatter_use;

		memcpy(vmsg->header->source, hp->source, 3);
		vmsg->header->text_format  = hp->text_format;
		memcpy(vmsg->header->destination, hp->destination, 3);
		vmsg->header->round_trip_info = hp->round_trip_info;
		vmsg->header->batch_number = hp->batch_number;
		memcpy(vmsg->header->reserved, hp->reserved, 3);
		vmsg->header->user_info = hp->user_info;
	}


	return(0);
}



extern_print_header(struct cardcoexternheader *hp)
{
	debug("len: %x\n", hp->len);
	debug("header_format: %x\n", hp->header_format);
	debug("text format: %x\n", hp->text_format);
	debug("message length: %02x%02x\n",
	      hp->message_length[0], hp->message_length[1]);
	debug("destination: %02x%02x%02x\n", hp->destination[0],
	      hp->destination[1], hp->destination[2]);
	debug("source: %02x%02x%02x\n", hp->source[0],
	      hp->source[1], hp->source[2]);
	debug("round trip info: %x\n", hp->round_trip_info);
	debug("base 1 flags: %02x%02x\n",
	      hp->base_1_flags[0], hp->base_1_flags[1]);
	debug("message status flags (N/A)\n");
	debug("batch number: %x\n", hp->batch_number);
}



extern_save_information(struct cardco *cardcomsg, register struct cardcoexternmsg *vmsg)
{
	register struct	private_data	*pd;

	if(cardcoIs03xx(cardcomsg))
		pd = (struct private_data *) ((struct cardco300 *)&cardcomsg->msg)->formatter_use;
	else
		pd = (struct private_data *)cardcomsg->msg.formatter_use;

	/* Save my bit indicators */
	memcpy(pd->bitmap, vmsg->bitmap, sizeof(pd->bitmap));

	/* save header information */
	pd->text_format = vmsg->header->text_format;
	memcpy(pd->source, vmsg->header->destination, 3);

	memcpy(pd->destination, vmsg->header->source, 3);
	pd->round_trip_info = vmsg->header->round_trip_info;
	pd->batch_number = vmsg->header->batch_number;
	memcpy(pd->reserved, vmsg->header->reserved, 3);
	pd->user_info = vmsg->header->user_info;

	/* save the network id bitmap (bit 63) */
	memcpy(pd->netidbitmap, v_NetworkIdBitmap, 2);

	return(0);
}



