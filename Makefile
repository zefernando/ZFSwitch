ORADIR=/u01/oracle/9.2.0
CFLAGS=-I. -I$(ORADIR)/precomp/public
ORALIBS=-L$(ORADIR)/precomp/lib/ -L$(ORADIR)/lib -L /usr/lib
SQL_LIB=-lproc2 -lclntsh
CC=cc


PRODUCAO=./

OBJ =	v_data.o v_out.o v_in.o v_tocardco.o  v_tonet.o \
		bitmap.o v_misc.o v_util.o v_header.o fila.o money.o storage.o 
SOURCES	=	v_data.c v_out.c v_in.c v_tocardco.c v_tonet.c \
		bitmap.c v_util.c v_misc.c fmt.c v_header.c fila.c money.c\
		storage.c  extern.h

all:	fmt log auth paybal tcpclient launch

fmt : fmt.o $(OBJ)
	$(CC) -g  -o $(PRODUCAO)fmt fmt.o $(OBJ)    

log : log.o fila.o
	$(CC) -g  -o $(PRODUCAO)log log.o fila.o   
	
auth : auth.o fila.o v_misc.o v_data.o v_util.o autorizacao.o
	$(CC) -g -o $(PRODUCAO)auth auth.o autorizacao.o $(OBJ) $(ORALIBS) $(SQL_LIB)

paybal : paybal.o fila.o v_misc.o v_data.o v_util.o pagamento.o
	$(CC) -g -o $(PRODUCAO)paybal paybal.o pagamento.o $(OBJ) $(ORALIBS) \
	$(SQL_LIB)
	
tcpclient:	tcpclient.o sockhelp.o fila.o v_misc.o v_data.o
	$(CC) -g -o tcpclient tcpclient.o sockhelp.o fila.o v_misc.o v_data.o  \
	-l pthread
	
tcpserver:	tcpserver.o sockserver.o fila.o v_misc.o v_data.o
	$(CC) -g -o tcpserver tcpserver.o sockserver.o fila.o v_misc.o v_data.o  \
	-l pthread
	
launch : launch.o 
	$(CC) -g  -o $(PRODUCAO)launch launch.o 

.c.o:
	$(CC) -c $(CFLAGS) $*.c

$(OBJ) ::	extern.h

clean:
	rm *.o

