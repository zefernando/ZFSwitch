
#ifndef CARDCO500STD_INCLUDE
#define CARDCO500STD_INCLUDE

#define CARDCO500STD_MAILBOX		"StdSettlement"
#define CARDCO500STD_RULENAME		"settlement-tables"

#define CARDCO500STD_NUM_OF_DAYS_TO_STORE		3
#define CARDCO500STD_YESTERDAY					0
#define CARDCO500STD_TODAY						1
#define CARDCO500STD_TOMMOROW					2


#define	CARDCO_NETWORK_DATA_LEN	30


/*	Types of messages we support:	anything else is a on-line request */
#define CARDCO500_MSG_INQUIRE					1500
#define CARDCO500_MSG_DAYEND					1501
#define CARDCO500_MSG_TRANSACTION				1502
#define CARDCO500_MSG_UPDATE_DB				1503




/*	Flags:		key.flags: on all entries */
#define CARDCO500_INUSE						0x00000001
#define CARDCO500_NEEDS_UPDATE					0x00000002


/*	Values for cardco500std.flags */
#define CARDCO500_TXN_REVERSAL					0x00000001


#include "std500.h"
#define cardco500stddb std500


struct	cardco500std {
	/*	Standard 500 message (ISO & X9.2) */
	
	/*	this MUST be the first field: To match with the cardcomsg structure */
	int		msgtype;
	
	int			index;			/*	internal pointer used by cardco500std.c */

	long		trace;
	date_t		trandate;
	long		trantime;
	long		flags;
	int			unit;			/*	required on return trip by formatter */

	date_t		settlement_date;

	bin_t		forward_inst;
	bin_t		issuer;
	bin_t		acquirer;
	bin_t		settlement_agent;

	short		currency_code;
	long		advice_reason;
	long		respcode;
	long		serial_number;

	char		network_data[CARDCO_NETWORK_DATA_LEN + 1];

	long		num_credit;
	long		num_credit_rev;

	long		num_debit;
	long		num_debit_rev;

	long		num_transfer;
	long		num_transfer_rev;

	long		num_inquiry;

	long		num_auth;
	long		num_auth_rev;

	/*	fees */
	amount_t	fee_cr_processing;
	amount_t	fee_cr_transaction;
	amount_t	fee_db_processing;
	amount_t	fee_db_transaction;


	/*	amounts */
	amount_t	amt_credit;
	amount_t	amt_credit_rev;
	amount_t	amt_debit;
	amount_t	amt_debit_rev;
	amount_t	amt_auth;
	amount_t	amt_auth_rev;


	amount_t	amt_netset;



};

/*
 *	Structures for the rule database
 */
struct	cardco500std_key {
	bin_t		issuer;				/*	settlement for this bin */
	int			textid;				/*	where the text starts */
	int			flags;				/*	flags on this entry */
};

struct	cardco500std_text {
	int			flags;				/*	flags on this entry */
	struct	cardco500std	msg;		/*	settlement information */
};





#define	cardco500StdNumKeys		cardco500StdRuleKeyp->keysused
#define cardco500StdNumText		cardco500StdRuleKeyp->textused
#define cardco500StdMaxKeys		cardco500StdRuleKeyp->nkeys

#define cardco500MarkEntryChanged(k)		((k)->flags |= CARDCO500_NEEDS_UPDATE)
#define cardco500MarkEntryClean(k)			((k)->flags &= ~CARDCO500_NEEDS_UPDATE)
#define cardco500EntryIsChanged(k)			((k)->flags & CARDCO500_NEEDS_UPDATE)

struct	cardco500std				*cardco500FindEntry();

#define cardcoIs5xx(m)							((m / 100) == 5)	/* XXXXXXXX */



#endif
