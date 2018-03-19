
#ifndef CARDCO300_INCLUDE
#define CARDCO300_INCLUDE

/*****************************************************************************
 *	Define server name
 *****************************************************************************/
#define CARDCO_300_SERVER		"03xxServer"


/*****************************************************************************
 *	Define the server structure
 *****************************************************************************/
struct cardco300 {

	/***	MUST MATCH EXACTLY AS IN 'struct cardcomsg' ***/

	int		msgtype;			/*	the message type */

	char	pan[CARDCO_PAN_LEN + 1];	/*	pan of card */
	long	pcode;					/*	processing code */

	/***	END OF MATCH SEQUENCE ***/


	date_t			trandate;
	long			trantime;
	long			trace;
	bin_t			acquirer;
	bin_t			issuer;
	int				respcode;
	int				senderid;
	int				unit;
	unsigned char	formatter_use[CARDCO_FORMATTER_USE];
	long			serial_1;
	long			serial_2;
	char			refnum[CARDCO_REFNUM_LEN + 1];
	char			file_security_code[3];
	char			file_name[18];
	double			dummy;
	char			action[256];
};


struct	cardco300fileupd {
	/*	Card Issuer UPDate 
		Redefines the 'action' field
	*/

	int				updcode;
	char			pan[CARDCO_PAN_LEN + 1];
	date_t			purge_date;
	char			action[4];
	char			region[20];
	char			special_processing[50];
};


struct	cardco300pvv {
	/*
	 *	Redefines 'action' field
	 *
	 *	PVV Update transaciton
	 */
	int				updcode;
	char			pan[CARDCO_PAN_LEN + 1];
	date_t			purge_date;
	char			algo[4];
	char			security_info[50];	/*	pin */
};


/*****************************************************************************
 *	Define private 0300 message: Cannot be mapped unto cardco
 *****************************************************************************/
struct cardco300priv {

	/***	MUST MATCH EXACTLY AS IN 'struct cardcomsg' ***/

	int		msgtype;			/*	the message type */

	char	pan[CARDCO_PAN_LEN + 1];	/*	pan of card */
	long	pcode;					/*	processing code */

	/***	END OF MATCH SEQUENCE ***/


	date_t			trandate;
	long			trantime;
	date_t			local_date;
	long			local_time;
	long			trace;
	bin_t			acquirer;
	bin_t			issuer;
	int				respcode;
	int				senderid;
	int				unit;
	unsigned char	formatter_use[CARDCO_FORMATTER_USE];
	long			serial_1;
	long			serial_2;
	char			refnum[CARDCO_REFNUM_LEN + 1];
	char			file_security_code[3];
	char			file_name[18];
	double			dummy;
	char			action[512];
};
#endif
