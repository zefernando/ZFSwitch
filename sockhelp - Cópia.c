#include "sockhelp.h"
char errMsg[512];
void DumpMsg(char *bp, int len, char *msg);

/* Take a service name, and a service type, and return a port number.  If the
   service name is not found, it tries it as a decimal number.  The number
   returned is byte ordered for the network. */
int atoport(char * service, char * proto)
{
	int port;
	long int lport;
	struct servent *serv;
	char *errpos;

	/* First try to read it from /etc/services */
	serv = getservbyname(service, proto);
	if (serv != NULL)
		port = serv->s_port;
	else { /* Not in services, maybe a number? */
		lport = strtol(service,&errpos,0);
		if ( (errpos[0] != 0) || (lport < 1) || (lport > 65535) )
			return -1; /* Invalid port address */
		port = htons(lport);
	}
	return port;
}

/* Converts ascii text to in_addr struct.  NULL is returned if the address
   can not be found. */
struct in_addr *atoaddr(char * address)
{
	struct hostent *host;
	static struct in_addr saddr;

	/* First try it as aaa.bbb.ccc.ddd. */
	saddr.s_addr = inet_addr(address);
	if (saddr.s_addr != -1) {
		return &saddr;
	}
	host = gethostbyname(address);
	if (host != NULL) {
		return (struct in_addr *) *host->h_addr_list;
	}
	return NULL;
}


/* This is a generic function to make a connection to a given server/port.
   service is the port name/number,
   type is either SOCK_STREAM or SOCK_DGRAM, and
   netaddress is the host name to connect to.
   The function returns the socket, ready for action.*/
int make_connection(char *service,char *netaddress)
{
	/* First convert service from a string, to a number... */
	int port = -1;
	struct in_addr *addr;
	int sock = -1, connected;
	struct sockaddr_in address;

	port = atoport(service, "tcp");

	if (port == -1) {
		logMsg("make_connection:  Invalid socket type.\n");
		return -1;
	}
	addr = atoaddr(netaddress);
	if (addr == NULL) {
		logMsg("make_connection:  Invalid network address.\n");
		return -1;
	}

	memset((char *) &address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = (port);
	address.sin_addr.s_addr = addr->s_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	logMsg("Connecting to %s on port %d.\n",inet_ntoa(*addr),htons(port));

	connected = connect(sock, (struct sockaddr *) &address,
	                    sizeof(address));
	if (connected < 0) {
		perror("connect");
		return -1;
	}
	authenticate(port, sock);
	// fcntl(sock, F_SETFL, O_NONBLOCK);
	return sock;
}


int authenticate(int port, int sd)
  {
  char header[4];
  char S_CONREQ[]={ 0x40, 0x01, 0x02 };
  char S_CONCNF[]={ 0x40, 0x01, 0x10 };
  // printf("___________________________________________________________\n");
  // printf("Iniciando porta %d ...\n", htons(port) );
  write( sd , S_CONREQ , 3 );
  read( sd , header , 3 );
  header[3] = 0;
  if( strncmp( header, S_CONCNF, 3 ) == 0) {
    logMsg("\tConexao TCP/X.25 Proceda estabelecida\n");
    return(0);
    }
   else {
    logMsg("\tFalha na conexao TCP/X.25 Proceda\n");
    // DumpMsg( header, 5, "header");
    return(-1);
    }
  }


/* Recebe dados da rede. Primeiro recebe o tamanho e depois o resto dos dados */
int rcvFromNet(int sockfd,char * buf)
{
	char auxBuf[2];

	int this_read;
	int bytes_read;
	int tam = 0;
	int ret = -1;
	char errmsg[512];
	memset(auxBuf,0, sizeof(auxBuf));

	this_read = read(sockfd, auxBuf, 2);
	if(this_read != 2) {
		logMsg("Read Error %s", strerror_r(errno,errmsg,512));
	}
	tam = (int )auxBuf[1];
	if(tam == 0 ) return -1;
	if(auxBuf[0] == 0x40) {
	/* Obtem o tamanho do bloco */
		this_read = read(sockfd, auxBuf, tam);
		ret =  -1;
	} 
	else {
		bytes_read = 0;
		while(auxBuf[0] == 0x20 ) {
			tam = (int)auxBuf[1];
			this_read = read(sockfd, buf + bytes_read, tam );
			if(this_read != tam) {
				logMsg("Read Error %s\n",strerror_r(errno,errmsg,512));
			}			
			if(this_read < 0) {
				logMsg("Read Error %s\n", strerror_r(errno,errmsg,512));
				ret =  -1;
				return ret;
			}

			bytes_read += this_read;
			this_read = read(sockfd, auxBuf, 2);
		}
    		if( auxBuf[0] == 0 ) {
	        	tam = (int)auxBuf[1];
			logMsg( "Ultimo fragmento da mensagem (bytes): %d\n" , tam );
			this_read = read(sockfd, buf + bytes_read, tam );
			if(this_read < 0) {
				logMsg("Read Error %s\n", strerror_r(errno,errmsg,512));
				ret =  -1;
			
			}
			else {
				bytes_read += this_read;
				ret = bytes_read;
			}
		}
	}
	return(ret);
}


/* Esta e a funcao de envio de dados. Ela usa a chamada write do S.O. */
int sendToNet(int sockfd, char * buf, size_t count)
{
	size_t bytes_sent = 0;
	int this_write;
	char auxBuf[2];
	/* printf("Count: %02x\n", count ); */
	auxBuf[0] = (char ) ((count & 0xff00) << 8);
	auxBuf[1] |= (char ) (count & 0x00ff);
	// DumpMsg(auxBuf,2,"Tamanho");
	this_write = write(sockfd, auxBuf, 2);
	if(this_write < 0) {
		printf("Erro no write %s\n", strerror(errno));
	} 
	// printf("bytes sent %d\n", this_write);
	// DumpMsg(buf,count,"send buf");
	while (bytes_sent < count) {
		do
			this_write = write(sockfd, buf, count - bytes_sent);
		while ( (this_write < 0) && (errno == EINTR) );
		if (this_write <= 0)
			break;
		bytes_sent += this_write;
		buf += this_write;
	}
	// printf("bytes sent %d\n", bytes_sent);
	if(this_write < 0) return -1;

	return bytes_sent;
}


void ignore_pipe(void)
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE,&sig,NULL);
}


FILE *  logOn(FILE *dbg, const char * filename)
{

	char path[80];
	sprintf(path, "./logDir/%s", filename);
	dbg = fopen(path,"a");
}

void logTcp(FILE * dbg, char *bp, int len, char *msg)
{
	int     i;
	char *p;
	p = bp;
	if(msg != NULL)
		fprintf(dbg, "******************  %s **************\n", msg);
	fprintf(dbg, "Tamanho: %d\n<", len);
	for(i = 0; i < len; ++i, ++p)
	{
		if(i >  0 && (i % 70) == 0)
			fprintf(dbg,"\n");
		fprintf(dbg, "%02x", ((int ) *p) & 0xff);
	}
	fprintf(dbg, ">\n");
	fflush(dbg);
}


void DumpMsg(char *bp, int len, char *msg)
{
	int     i;
	char *p;
	p = bp;
	if(msg != NULL)
		printf("******************  %s **************\n", msg);
	printf("Tamanho: %d\n<", len);
	for(i = 0; i < len; ++i, ++p)
	{
		if(i >  0 && (i % 70) == 0)
			printf("\n");
		printf("%02x", ((int ) *p) & 0xff);
	}
	printf(">\n");
}

