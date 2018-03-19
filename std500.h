/*
 *	IST and IST/Share are registered trademarks of Oasis Technology Ltd.
 *
 *	Copyright (C) 1990 - 1997 Oasis Technology Ltd.
 *	All Rights Reserved
 *	This Module contains Proprietary Information of
 *	Oasis Technology Ltd., and should be treated as Confidential.
 *
 *	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF OASIS TECHNOLOGY LTD.
 *	The copyright notice above does not evidence any  
 *	actual or intended publication of such source code.
 */

#ifndef STD500_H
#define STD500_H

#include <stdio.h>

struct std500 
{
	long		msgtype;
	long		trace;
	long		trandate;
	long		trantime;
	long		flags;
	long		settlement_date;
	char		forward_inst[11];
	char		issuer[11];
	char		acquirer[11];
	char		settlement_agent[11];
	short		currency_code;
	long		advice_reason;
	long		respcode;
	long		serial_number;
	char		network_data[31];
	long		num_credit;
	long		num_credit_rev;
	long		num_debit;
	long		num_debit_rev;
	long		num_transfer;
	long		num_transfer_rev;
	long		num_inquiry;
	long		num_auth;
	long		num_auth_rev;
	dec_t		fee_cr_processing;
	dec_t		fee_cr_transaction;
	dec_t		fee_db_processing;
	dec_t		fee_db_transaction;
	dec_t		amt_credit;
	dec_t		amt_credit_rev;
	dec_t		amt_debit;
	dec_t		amt_debit_rev;
	dec_t		amt_auth;
	dec_t		amt_auth_rev;
	dec_t		amt_netset;
	long		o_rowid;
};
typedef struct std500 STD500;

typedef struct std500 ist_Std500_t;

#endif

