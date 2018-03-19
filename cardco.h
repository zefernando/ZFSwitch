
#define CARDCO_FMT_DEBUGIN 1
#define	CARDCO_FMT_DEBUGOUT 1

/* Tipos de Mensagem */
#define CARDCO_AUTHREQ			100
#define CARDCO_AUTHREQ_REPEAT		101
#define CARDCO_AUTHCOMP			102
#define CARDCO_AUTHREQ_ADVICE		120
#define CARDCO_AUTHREQ_ADVICE_RESPONSE	130
#define CARDCO_AUTHREQ_RESPONSE		110
#define CARDCO_FINREQ			200
#define CARDCO_FINREQ_REPEAT		201
#define CARDCO_FINREQ_ADVICE		220
#define CARDCO_FINREQ_ADVICE_RESPONSE	230
#define CARDCO_FINREQ_RESPONSE		210
#define CARDCO_FILEUPD_ISSREQ		302
#define CARDCO_FILEUPD_ISSREQ_RESPONSE	312

#define CARDCO_REVREQ			400
#define CARDCO_REVREQ_REPEAT		401
#define CARDCO_REVREQ_ADVICE		420
#define CARDCO_REVREQ_ADVICE_RESPONSE	430
#define CARDCO_REVREQ_RESPONSE		410

#define CARDCO_STLMTREQ				500
#define CARDCO_STLMTREQ_RESPONSE		510
#define CARDCO_STLMTREQ_ADVICE			520
#define CARDCO_STLMTREQ_ISSUER_ADVICE		522
#define CARDCO_STLMTREQ_ADVICE_RESPONSE		530
#define CARDCO_STLMTREQ_ISSUER_ADVICE_RESPONSE	532
#define CARDCO_STLMTREQ_EDC			502
#define CARDCO_STLMTREQ_EDC_RESPONSE		512

#define CARDCO_ADMIN				600
#define CARDCO_ADMIN_RESPONSE			610
#define CARDCO_ADMIN_ADVICE			620
#define CARDCO_ADMIN_ADVICE_RESPONSE		630

#define CARDCO_NETWORK			800
#define CARDCO_NETWORK_RESPONSE		810


#define cardcoIsDeviceAck(cardcomsg)			(((cardcomsg)->msg.msgtype/1000 % 10) == 9)
#define cardcoIsAuthorizationRequest(cardcomsg)	(((cardcomsg)->msg.msgtype / 100) == 1)
#define cardcoIsFinancialRequest(cardcomsg)		(((cardcomsg)->msg.msgtype / 100) == 2)

#define cardcoIsFileUpdateRequest(cardcomsg)		(((cardcomsg)->msg.msgtype/100)%10==3)
#define cardcoIs03xx(cardcomsg)					cardcoIsFileUpdateRequest(cardcomsg)
#define cardcoIsReversal(cardcomsg)				(((cardcomsg)->msg.msgtype / 100) == 4)
#define cardcoIsSettlement(cardcomsg)				(((cardcomsg)->msg.msgtype / 100) == 5)
#define cardcoIsAdmin(cardcomsg)					(((cardcomsg)->msg.msgtype / 100) == 6)
#define cardcoIsRequest(cardcomsg)				(!cardcoIsResponse((cardcomsg)))
#define cardcoIsAdviceResp(cardcomsg) ((cardcomsg)->msg.msgtype == CARDCO_FINREQ_ADVICE_RESPONSE)
#define cardcoIsRevAdviceResp(cardcomsg) ((cardcomsg)->msg.msgtype == CARDCO_REVREQ_ADVICE_RESPONSE)
#define cardcoIsAdvice(cardcomsg) ((cardcomsg)->msg.msgtype == CARDCO_AUTHREQ_ADVICE || \
							 (cardcomsg)->msg.msgtype == CARDCO_FINREQ_ADVICE)

#define cardcoIsCustomerRequest(cardcomsg)\
	(cardcoIsAuthorizationRequest((cardcomsg)) || cardcoIsFinancialRequest((cardcomsg)))

/*	A response has an odd digit in the 10's position */
#define cardcoIsResponse(cardcomsg)		( ((cardcomsg)->msg.msgtype / 10 % 10) & 1)
#define cardcoIsNetwork(cardcomsg)		(((cardcomsg)->msg.msgtype / 100) == 8)
#define cardcoIsLogRequest(cardcomsg) \
	(((cardcomsg)->msg.msgtype - ((cardcomsg)->msg.msgtype % 1000)) == CARDCO_LOGREQ)
#define cardcoIsRevAdvice(cardcomsg) ((cardcomsg)->msg.msgtype == CARDCO_REVREQ_ADVICE)
#define cardcoIsATM(cardcomsg)	((cardcomsg)->msg.merchant_type == 6011)

/* Codigos de respostas */
#define	CARDCO_RC_Approved		0
#define	CARDCO_RC_ReferCardIssuer 	2
#define	CARDCO_RC_InvalidMerchant 	3
#define	CARDCO_RC_HotCardPickUp	  	4
#define	CARDCO_RC_DoNotHonor	   	5
#define	CARDCO_RC_UnableToProcess 	6
#define CARDCO_RC_IssuerTimeout		8
#define CARDCO_RC_NoOriginal		9
#define CARDCO_RC_UnableToReverse       10
#define	CARDCO_RC_InvalidTransaction 	12
#define CARDCO_RC_InvalidAmount 	13
#define CARDCO_RC_InvalidCard 		14

#define CARDCO_RC_SystemErrorReenter 	19

#define CARDCO_RC_RecordNotFound 	25
#define CARDCO_RC_FileNotAvailable 	28
#define CARDCO_RC_MessageFormatError	30
#define CARDCO_RC_HotCard 		41
#define	CARDCO_RC_SpecialPickup		42
#define CARDCO_RC_NoFunds 		51
#define CARDCO_RC_InvalidAccount 	52
#define CARDCO_RC_ExpiredCard 		54
#define CARDCO_RC_IncorrectPIN 		55
#define CARDCO_RC_TxnNotPermittedOnCard	57

#define CARDCO_RC_ExceedsLimit 		60
#define CARDCO_RC_RestrictedCard 	61
#define CARDCO_RC_MACKeyError 		63
#define CARDCO_RC_RetainCard		67
#define CARDCO_RC_ExceedsPINRetry 	75
#define CARDCO_RC_NoSharingArrangement	77
#define CARDCO_RC_FunctionNotAvailable	78
#define CARDCO_RC_InvalidCaptureDate 	80
#define CARDCO_RC_ExceedsFreqLimit 	82
#define CARDCO_RC_PINKeyError		87
#define CARDCO_RC_MACSyncError		88
#define CARDCO_RC_SwitchNotAvailable 	91
#define CARDCO_RC_InvalidIssuer		92
#define CARDCO_RC_InvalidAcquirer	93
#define CARDCO_RC_InvalidOriginator	94
#define CARDCO_RC_SystemError		96
#define	CARDCO_RC_NoFundsTransfer	97 /*Acct no funds during txf. 11Feb93*/
#define CARDCO_RC_DuplicateReversal	98
#define CARDCO_RC_DuplicateTransaction	99


#define CARDCO_RC_PartialDispense	102

/*	Codigos de motivos de desfazimento */
#define CARDCO_RR_CustomerCancel	17
#define CARDCO_RR_InvalidResponse	20
#define CARDCO_RR_HardwareError		22
#define CARDCO_RR_MACKeyError		30
#define CARDCO_RR_MACSyncError		31
#define CARDCO_RR_PartialDispense	32
#define CARDCO_RR_TimeOut		68
#define CARDCO_RR_DestinationDown	82
#define CARDCO_RR_CancelRequest		91
#define CARDCO_RR_RejectMessage		92
#define CARDCO_RR_UncertainDispense	93
#define CARDCO_RR_TransactionInDoubt	94
#define CARDCO_RR_DepositError		95

#define CARDCO_RC_RejectMessage		903

#define CC_DEVCAP_TRACK_1			0x00000002
#define CC_DEVCAP_NOTRACK_DATA			0x00000040

/* Modos de entrada dos POS */
#define	CC_POS_PANENTRY_UNKNOWN 	0
#define	CC_POS_PANENTRY_MANUAL		1
#define	CC_POS_PANENTRY_MAGSTRIP	2
#define	CC_POS_PANENTRY_BARCODE		3
#define	CC_POS_PANENTRY_OCR		4
#define	CC_POS_PANENTRY_IC		5
#define	CC_POS_PANENTRY_TRK1OR2		90

#define CC_POS_PINENTRY_UNKNOWN		0
#define CC_POS_PINENTRY_HASCAP		1	/*	terminal has capability */
#define CC_POS_PINENTRY_NOCAP		2	/*	no capability */
#define CC_POS_PINENTRY_INOPERATIVE	8	/*	has cap. but is inoperative */
#define CC_POS_PINENTRY_VERIFIED	9	/*	verified by device */


/*	The following fields are coded (bit coded) */
#define CC_POS_COND_TERMATOWNER		0x0001	/* if true then located at owner */
#define CC_POS_COND_CUSTPRESENT		0x0002	/* customer present */
#define CC_POS_COND_CARDPRESENT		0x0004	/*	card present */
#define CC_POS_COND_AVS				0x0008	/*	verfication request */

#define CC_POS_COND_REQ_MASK		0x00f0	/*	16 different req values */
#define CC_POS_COND_REQ_NORMAL		0x0010	/*	normal request */
#define CC_POS_COND_REQ_PREAUTH		0x0020	/*	pre-authorization */
#define CC_POS_COND_REQ_DOWNSUB		0x0030	/*	down time submission request */
#define CC_POS_COND_REQ_RESUB		0x0040	/*	re-submission request */
#define CC_POS_COND_REQ_MAIL		0x1000	/*	mail telephone order */
#define	CC_POS_COND_MERCHAUTH		0x0060	/*  merchant authorized */
#define	CC_POS_COND_CACCBACK		0x0070	/*  cashback */
#define CC_POS_COND_ECR			0x2000  /*  terminal is ECR */
#define CC_POS_COND_MANREV		0x0090  /*  Manual Reversal (by retailer)*/
#define CC_POS_COND_TERMERROR		0x0095  /*  Terminal Error  */
#define CC_POS_COND_SUSPECT		0x0100	/*	suspect fraud */
#define CC_POS_COND_IDENTITY_VER	0x0200	/*	identiy verified */
#define CC_POS_COND_AVS_AUTH		0x0400	/*	avs with authorization */
#define CC_POS_COND_CREDIT_TXN		0x8000	/*	to be set by formatter/switch */
/* Unattended terminal, customer operated */
#define CC_POS_COND_UNATTENDEDTERM	0x0800						/* 10001845 */

#define CC_POS_CAP_UNSPECIFIED		0x0001
#define CC_POS_CAP_UNATTENDED		0x0002	/*	terminal unattended */
#define CC_POS_CAP_CARDHOLDER_OPER	0x0004	/*	card holder operated terminal */
#define CC_POS_CAP_CARDCAPTURE		0x0008	/*	can capture cards */
#define CC_POS_CAP_DEBIT			0x0010	/*	can support debit cards */
#define CC_POS_CAP_CREDIT			0x0020	/*	can support credit cards */
#define CC_POS_CAP_NOCARDREAD		0x0040	/*	can read card data */
#define CC_POS_CAP_ECR				0x0080	/*	terminal is ECR */
#define CC_POS_CAP_DIALTERM			0x0100	/*	dial terminal */
#define CC_POS_CAP_CARDHOLDER_UNSP	0x0200	/*	card hldr oper. unspesif. */
#define CC_POS_CAP_CONF_PENDING		0x0400	/*	this is for a confirm. of a 
												pending txn.	*/
#define CC_POS_CAP_ACK_PROCESSED	0x0800	/*	9000/280 processed at another node already */
#define	CC_POS_CAP_POSTERM			0x1000	/*  pos terminal */
#define	CC_POS_CAP_SCRIPDEV			0x2000	/*	scrip device */


/* Tipos de mensagem 800 (NetWork ) */

#define	CARDCO_NET_SIGNON	1
#define	CARDCO_NET_SIGNOFF	2

#define CARDCO_NET_START_SAF	60
#define CARDCO_NET_STOP_SAF	61

#define CARDCO_NET_ENTER_SI	62
#define CARDCO_NET_EXIT_SI	63

#define	CARDCO_NET_ECHO		301
#define CARDCO_NET_ECHO_START	9301
/*	Internal numbers	*/
#define CARDCO_NET_SIGNON_START		9001
#define CARDCO_NET_SIGNOFF_START	9002


#define CARDCO_NET_KEYS		161

#define cardcoGetProcessCode(pcode)	((pcode) / 10000)


/* PROCESSING CODES*/

/* goods or services purchase */
#define	CC_TX_SERVICES 0
/* Saque */
#define	CC_TX_WD 1
/* Garantia de cheque */
#define CC_TX_CHECK_GUARANTEE 3
/* Consulta de saldo */
#define CC_TX_BAL 30
/* Devolucao de mercadoria */
#define CC_TX_DEP 20
/* Transferencia de fundos */
#define CC_TX_TSF 40
/* Pagamento de contas */	
#define CC_TX_BILLPAY 82

/* Conta origem */
/* Poupanca */
#define CC_ACCT_SAV 10
/* Conta Corrente */
#define CC_ACCT_CHEQ 20
#define CC_ACCT_CHECK			CC_ACCT_CHEQ
/* Conta Corrente */
#define CC_ACCT_CC 30

#define CC_ACCT_UNIVERSAL		40

#define CC_ACCT_CC_0 0
#define CC_ACCT_DEFAULT	0

/*		These are the ISO equivilents cardcomsg.msg.pcode */
/*	account debit */
#define CC_ISO_WITHDRAW_CLASS		0
#define CC_ISO_WITHDRAW_CLASS2		1

#define CC_ISO_G_AND_S			0
#define	CC_ISO_WD			1
#define CC_ISO_DEBIT_ADJUST		2
#define CC_ISO_CHECK_GUAR		3
#define CC_ISO_CHECK_VER		4

#define CC_ISO_TEST_DEBIT		5
#define CC_ISO_TEST_DEBIT_2		6

#define CC_ISO_G_AND_S_CASH		9
/* 18May93 :start*/
#define CC_ISO_PREAUTHREQ		10
#define CC_ISO_PREAUTHRESP		10
#define CC_ISO_PREAUTHCONF		11
/* 18May93 :End*/
#define CC_ISO_TC_MONEY_ORDER		14
#define CC_ISO_TC			16		/*	traveler cheques */
#define	CC_ISO_CACC_ADVANCE		17
#define CC_ISO_SCRIP_ISSUE		18
#define CC_ISO_POSDEBIT			19	/* 13mar93 */

/*	account credit */
#define CC_ISO_DEPOSIT_CLASS	2
#define CC_ISO_PAYMENT_CLASS	CC_ISO_DEPOSIT_CLASS
#define	CC_ISO_PURCH_RETURN		20
#define CC_ISO_DEPOSIT			21
#define CC_ISO_CREDIT_ADJUST	22
#define CC_ISO_CHECK_DEP_GUAR	23
#define CC_ISO_CHECK_DEP		24
#define CC_ISO_LOAN_DEP			25
#define CC_ISO_ENV_PAYMENT		28
#define	CC_ISO_CORRECTION		29

/* 15Sep93 */
#define CC_ISO_POSCREDIT		50

/*	inquiries */
#define CC_ISO_BALANCE_CLASS	3
#define CC_ISO_BALANCE			30

/* 05jul93 */
#define	CC_ISO_INQACCT			35

/* 24nov92 */
#define CC_ISO_CARDVALIDATE		36

#define CC_ISO_BALFILE1			37
#define CC_ISO_BALFILE2			38
#define CC_ISO_BALFILE3			39		/*	user defined balance inquiry
								The net effect is to transfer
								the information to the driver
								in a known file location. The
								information in the file should
								be pre-formatted.
							*/

/*	account transfers */
#define CC_ISO_TRANSFER_CLASS	4
#define CC_ISO_TRANSFER			40
#define CC_ISO_LOAN_TSF			41
#define CC_ISO_TRANSFER_CNF_1	41		/* 08jul94 */
#define CC_ISO_PAYMENT_ACCT		42
#define CC_ISO_TRANSFER_CNF_2	48		/* 08jul94 */
#define CC_ISO_TRANSFER_OTHER	49	/* tsf into other account (same inst) */


#define	CC_300_FILE_UPDATE			1		/*	03xx file update */
#define CC_300_PVV_UPDATE			2		/*	03xx pvv update */
#define CC_300_FILE_INQ				3
#define CC_300_PVV_INQ				4

#define cardcoIsPurchWithCash(pcode)      ((pcode) / 10000 == CC_ISO_G_AND_S_CASH)

/* Definicoes de formatos */
#define FMT_CHAR 1
#define FMT_BTOC 2
#define FMT_BTOI 3
#define FMT_BTOS 4
#define FMT_BTOM 5
#define FMT_BINARY 6 
#define FMT_BTOR 7
#define FMT_CTOM 8
#define FMT_ATOI 9
#define FMT_MOVE 10
#define FMT_ITOA 11

/*
 *	Define the types
 */

/* typedef struct decimal dec_t; */
typedef char	dec_t[13];
typedef char	dth_t[13];

typedef dec_t	amount_t;		/*	amount type */
typedef	char	bin_t[11];		/*	bin type */
typedef	long	date_t;

struct  _seckey_t {
	char	key[16];		/*	the key */
	char	chk[4];			/*	the check digits */
};

typedef	struct	_seckey_t	seckey_t;


#define CARDCO_PAN_LEN		19			/*	pan length MAX */
#define CARDCO_TRACK2_LEN	79			/*	length of track two on card */
#define CARDCO_TRACK3_LEN	104			/*  	track three length */
#define CARDCO_REFNUM_LEN	12			/*	reference retrieval number */
#define CARDCO_AUTHNUM_LEN	6			/*	issuers auth. number */
#define CARDCO_TERMID_LEN	8			/*	terminal id length */
#define CARDCO_ACCEPT_LEN	40			/*	acceptor name and location */
#define CARDCO_PIN_LEN		16			/*	pin length */
#define CARDCO_TERMLOC_LEN	25			/*	terminal location */
#define CARDCO_MACVALUE_LEN	16			/*	mac value on each message */
#define CARDCO_BIN_LEN		10			/*	length of a BIN number */
#define CARDCO_FORMATTER_USE	64
#define CARDCO_ADDRESPONSE_LEN	25

#define CARDCO_ACCT_LEN		42	 		/*	additional or returned acct # */
#define CARDCO_BANKNAME_LEN	22
/*
 *  Filler spaces to store more info
 */
#define CARDCO_FILLER_LEN	50

/*
 *     SMS issuer & acquirer data length
 */
#define SMS_DATA_LEN            128

#define CARDCO_SEG_MAX		2048
bin_t	cardco_orgid;
int	cardcomid;

#define cardco_xday(d)		((d) % 100)
#define cardco_xmon(d)          ((d) / 100 % 100)
#define cardco_xyear(d)    	((d) / 10000L)
#define cardco_xcentury(d) 	( (d) / 1000000L)
#define cardco_xfullyear(d)     ( (d) / 10000L)
#define cardco_getcentury(d)	(cardco_xfullyear((d)) * 10000L)

/*
 *	Define the universal shared cash message
 */
struct	internalmsg {
	int			msgtype;		/*	the message type */
	char		pan[CARDCO_PAN_LEN + 1];	/*	pan of card */
	long		pcode;				/*	processing code */
	/*	Internaly coded values */
	unsigned long	txntype;
	long		from_acct;
	long		to_acct;
	/*	these two fields are always in local currency:
		when we return to the device they are in the currency of the
		acquirer, when we forward to the issuer they are in the currency
		of the issuer.
	*/
	amount_t	amount;			/*	primary amount of transaction */
	amount_t	aval_balance;		/*	available balance in issuer currency */
									
	amount_t	ledger_balance;		/*	ledger balance */
	amount_t	cash_back;		/*	cash back amount (if any) */

	/*	cambio */
	amount_t	iss_conv_rate;		/*	conversion rate as applied to txn */
	short		iss_currency_code;	/*	currencry of tranasction */
						/*	at issuer side */
	date_t		iss_conv_date;		/*	date conversion rate is good at */

	amount_t	acq_conv_rate;		/*	conversion rate as applied to txn */
	short		acq_currency_code;	/*	currencry of tranasction */
						/*	at issuer side */
	date_t		acq_conv_date;		/*	date conversion rate is good at */
	
	amount_t	tra_amount;		/*	amount in ccy of the credit acct */
	amount_t	tra_conv_rate;		/*	conversion rate as applied to txn */
	short		tra_currency_code;	/*	currencry of tranasction */
						/*	at transferee side */
	date_t		tra_conv_date;		/*	date conversion rate is good at */

	amount_t	amount_equiv;		/*	the value of the transaction amount */
						/*	in the currency of the terminal */
	amount_t	fee;
	amount_t	new_fee;		 
	amount_t	new_amount;	    	/*  	the replacement amount in 400 */ 
	amount_t	new_setl_amount;         
	amount_t	settlement_fee;		 
	amount_t	settlement_rate;	/*	conversion rate of settlement amount */
	short		settlement_code;	/*	currency code of settlement */
	amount_t	settlement_amount;	/*	amount in settlement currency */
	date_t	trandate;			/*	transmission date */
	long	trantime;			/*	transmission time */
	long	trace;				/*	system trace number */
	long	local_time;			/*	terminal local time */
	date_t	local_date;			/*	terminal local date */
	date_t	settlement_date;		/*	settlement date */
	date_t	cap_date;			/*	capture date */

	/*	service codes */
	short	pos_entry_code;			/*	point of service code */
	short   pos_condition_code;
	unsigned char pos_pin_cap_code;
	short	pos_cap_code;
	char	pos_geo_loc[15];
	long	life_cycle;		/*	expressed in number of seconds */
	bin_t	acquirer;		/*	acquiring institution id */
	bin_t	issuer;			/*	issuing institution id */
	bin_t	transferee;		/*	transferee institution id */
	bin_t	originator;		/*	switch id which sent message */
	int	respcode;		/*	respcode on message */
	int	reason_code;		/*	reason code */
	int	revcode;		/*	reversal code */
	int	cardcoerror;		/*	internal processing code */
	/*	Reversal data elements */
	int		origmsg;	/*	original message type */
	int		origtrace;	/*	original trace number */
	date_t	origdate;		/*	original date */
	long	origtime;		/*	original time */

	/*	fields for extern / master card */
	short	merchant_type;
	short	acq_country;
	short	card_seqno;
	long	serial_1;
	long	serial_2;
	int	origrespcode;

	char	track2[CARDCO_TRACK2_LEN+1];		/*	track 2 data */
	char	track3[CARDCO_TRACK3_LEN+1];		/*	track 3 data */
	char	refnum[CARDCO_REFNUM_LEN+1];		/*	referance number */
	char	authnum[CARDCO_AUTHNUM_LEN+1];		/*	issuers authorization number */
	char	termid[CARDCO_TERMID_LEN+1];		/*	terminal identification */
	char	acceptorname[CARDCO_ACCEPT_LEN+1];	/*	acceptor name and location */
	char	termloc[CARDCO_TERMLOC_LEN+1];		/*	terminal location */
	char	addresponse[CARDCO_ADDRESPONSE_LEN + 1];
	char	acctnum[CARDCO_ACCT_LEN + 1];
	/*
	 *	Security information	as per RBC ISO8583
	 */
	char	pin[CARDCO_PIN_LEN];	/*	encrypted pin */
	char	format_code[2];
	char	pin_algo[2];
	char	pin_index;
	char	mac_index;
	int	netcode;		/*	network mgmnt code */
	seckey_t	mac;		/*	mac on this message */
	int		device_cap;	/*	device capabilities */
	short	branch;			/*	branch number */
	/*	Internal Stuff */
	int		senderid;	/*	sender of message */
	unsigned char	formatter_use[CARDCO_FORMATTER_USE];
	short	saf;			/*	saf indicator */
	int		unit;		/* Unit number of the ATM we are dealing with */
	long	device_devcap;
	long	formatter_devcap;
	long	cardco_devcap;
	long	auth_devcap;
	/*	stats stuff */
	long	txn_entry_time;		/*	when txn entered the system */
	long	txn_start_time;		/*	first time txn entered the system */
	unsigned long		storeid;
	unsigned long		lane;
	unsigned long		terminal_trace;
	unsigned long		checker_id;
	unsigned long		supervisor;
	unsigned short		shift_number;
	long 				batch_id;
	amount_t	amount1;	
	amount_t	amount2;	
	amount_t	amount3;	
	amount_t	amount4;	
	amount_t	amount5;	
	amount_t	amount6;	
	amount_t	amount7;	
	amount_t	amount8;	
	amount_t	amount9;	
	dth_t		dth_upd;	
	char	filler1[CARDCO_FILLER_LEN];
	char	filler2[CARDCO_FILLER_LEN];
	char	filler3[CARDCO_FILLER_LEN];
	char	filler4[CARDCO_FILLER_LEN];
	long	origpcode;				/*	original processing code */
        char    issuer_data[SMS_DATA_LEN];
	char    acquirer_data[SMS_DATA_LEN];
	/* 
	*       Suporte a multiplas moedas
	*/
	amount_t	new_amount_equiv;		/* replacement amount in issuer currency */
	amount_t	acq_aval_balance;		/* aval_balance in acquirer currency */
	amount_t	acq_ledger_balance;		/* ledger_balance in acquirer currency */
	amount_t	setl_aval_balance;		/* aval_balance in settlement currency */
	amount_t	setl_ledger_balance;		/* ledger_balance in settlement currency */ 
 	short		aval_balance_type;		/* aval_balance amount type */
	short		ledger_balance_type;		/* ledger_balance amount type */
	amount_t	new_setl_fee;			/* fee in the event of a partial reverssal */
	amount_t	txn_amount;			/* txn amount in transaction currency */
	amount_t	txn_new_amount;			/* replacement amount in transaction currency */
	short		txn_currency_code;		/* currency code of transaction */
	amount_t	txn_conv_rate;			/* conversion rate applies in acquirer side */
	date_t		txn_conv_date;			/* date txn_conv_rate is good at */
	amount_t	ch_amount;			/* txn amount in CH's billing currency */
	amount_t	ch_new_amount;			/* replacement amount in CH's billing ccy */
	short		ch_currency_code;		/* CH's billing currency code */
	amount_t	ch_conv_rate;			/* conversion rate applies in issuer side */
	date_t		ch_conv_date;			/* date ch_conv_rate is good at */
};

struct cardco {
	char header[50];
	struct internalmsg msg;
};

#include "cardco300.h"
#include "cardco500std.h"
