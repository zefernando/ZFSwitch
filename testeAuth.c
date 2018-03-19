#include <stdio.h>
#include <cardco.h>
#include <extern.h>
#include <fila.h>
struct internalmsg *msg;
struct cardco *cardcoPtr;
main(int argc, char **argv)
{
/*
Bits found:[2,3,4,7,14,18,19,22,25,32,37,41,42,43,49,60,]
msgno[  0]<100>
Bitmap: [7204648108e08010]
field 2 6275760000014005 len 16
field 3  0 len 6
field 3.00 len 6
field 7  330 len 4
field 7  114941 len 6
field 14  609 len 4
field 5411 len 4
field 76 len 4
field 100 len 4
field 0 len 2
field 32 787878 len 6
field 37  len 12
field 41  len 8
field 42  len 15
field 43  len 40
field 98 len 3
field 1 len 2
*/
	cardcoPtr = &cardcomsg;
msg     = &cardcoPtr->msg;
	msg->msgtype = 100;
	strcpy(msg->pan, "6275760000014005");
	msg->pcode = 0;
msg->local_date = 402;
msg->local_time = 83430;
	strcpy(msg->track2, "=0609");
	strcpy(msg->acquirer, "787878");
	strcpy(msg->amount, "000000000800");

	printf("msgtype = %d\n", msg->msgtype);
        printf("pan=<%s>\n", msg->pan);
        printf("pcode = %d\n", msg->pcode);
        printf("issuer = <%s>\n", msg->issuer);
        printf("acquirer = <%s>\n", msg->acquirer);


        fflush(stdout); 

	msg->trandate = 402;
        printf("trandate = <%d>\n", msg->trandate);


        printf("trace = %d\n", msg->trace);
	msg->trace = 11111;

        fflush(stdout);

        printf("local_time = %d\n", msg->local_time);

        printf("local_date = %d\n", msg->local_date);

        printf("Expiry date in Track-II=<%s>\n", msg->track2);

        printf("settlement_date = %d\n", msg->settlement_date);
	dumpMsg((char *) msg, sizeof(struct internalmsg), "TESTE");

gravaFila(AUTHQ,(char *) msg,sizeof(struct internalmsg)) ;



}
