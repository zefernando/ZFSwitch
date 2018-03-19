#ifndef CARDCOEXT_INCLUDE
#define CARDCOEXT_INCLUDE


#define EXT_HEADER_LEN			22
#define EXT_REJECT_HEADER_LEN		26
#define EXT_BITMAP_LEN			8
#define EXT_MESSAGE_NUMBER_LEN		2
#define	EXT_MESSAGE_LEN		1024
#define FORMATTER_NAME	"Formatador Visa"
#define EXT_BITMAP_SIZE		16

#define	MSG_ATM				0x1
#define	MSG_POS				0x2
#define MSG_BOTH			(MSG_ATM | MSG_POS)

/*
 *	Define saved bit locations for use in response messages
 */
#define SAVED_BIT_LOCATION		(sizeof(struct cardcoexternheader_reject))
#define SAVED_BITS_SIZE			2	
#define	S_BIT2					0
#define S_BIT3					1
#define S_BIT4					2
#define S_BIT10					3
#define S_BIT11					4
#define S_BIT23					5
#define S_BIT41					6
#define S_BIT42					7
#define S_BIT95					8




#define	uchar	unsigned char


/*
 *	Define extern bit terminology
 */
#define	V_BIT1			0x80
#define V_BIT2			0x40
#define V_BIT3			0x20
#define V_BIT4			0x10
#define V_BIT5			0x08
#define V_BIT6			0x04
#define V_BIT7			0x02
#define V_BIT8			0x01


extern	uchar	bitmp62 [ ];
extern	int fmt_debug;
extern	char	*argv0;

struct	private_data {    /* redefines cardco.formatter_use[] */
	char	netidbitmap[2];
	uchar	bitmap[16];
	uchar	destination[3];
	uchar	source[3];
	uchar	round_trip_info;
	uchar	batch_number;
	uchar	reserved[3];
	uchar	user_info;
	uchar	have_large_bin;
	uchar	large_bin[12];
	uchar	length62;
	uchar	bitmp621;
	uchar	text_format;
	uchar	validation_code [5];
};


struct cardcoexternheader {
	uchar	len;
	uchar	header_format;
	uchar	text_format;
	uchar	message_length[2];
	uchar	destination[3];
	uchar	source[3];
	uchar	round_trip_info;
	uchar	base_1_flags[2];
	uchar	message_status_flags[3];
	uchar	batch_number;
	uchar	reserved[3];
	uchar	user_info;
};

struct	cardcoexternheader_reject {
	struct	cardcoexternheader	header;
	uchar	bitmap[2];
	uchar	reject_data_group[2];
};

struct	cardcoexternmsg {
	struct	cardcoexternheader	*header;
	uchar	*msgno;
	uchar	*bitmap;
	uchar	*msg;
	uchar	isreject;
	uchar	pin_found;
	uchar	mac_found;
	uchar	replacement_amount;
	uchar	trace_found;
};


struct	save302 {
	/* Used to save routing information during a 302/312 transfer */
	int	origmbid;   /*	who do we return message to */
	uchar	origuf;	    /*	original user info */
	uchar	uf;	    /*	our generated user info */
};



struct	msgmap {
	uchar	*map;
	int	*idx;
	int	msgno;
	int	type;
};

/*
 *	Globals
 */

extern		int		pid;
extern		struct		cardcoexternmsg	externmsg;
extern		uchar		externbuf[EXT_MESSAGE_LEN];
extern		struct		cardco	cardcomsg;
extern		uchar		source_station_id[];
extern		short		extern_currency;
extern		char		v_NetworkIdBitmap[8];
extern		long		cardco_current_istdate;
extern		bin_t		MIPid;
extern		int		extern_support300;
extern		int		extern800debug;
extern		int		msgType;
extern		int		bitno;


extern	unsigned char *find_bitmap(register int msgno, register int type);
extern	unsigned char *extern_getbitmap(struct cardco *cardcomsg);



#endif
