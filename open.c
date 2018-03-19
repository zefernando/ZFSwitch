int OpenConn(int port, int sd)
  {
  char header[3];
  char S_CONREQ[]={ 0x40, 0x01, 0x02 };
  char S_CONCNF[]={ 0x40, 0x01, 0x10 };
  printf("___________________________________________________________\n");
  printf("lambda: Iniciando porta %d ...\n", port );
  write( sd , S_CONREQ , 3 );
  so_read( sd , header , 3 );
  if( strncmp( header, S_CONCNF, 3 ) == 0) {
    printf("\tConexao TCP/X.25 Proceda estabelecida\n");
    return(0);
    }
   else {
    printf("\tFalha na conexao TCP/X.25 Proceda\n");
    hexdump( header, 5);
    return(-1);
    }
  }

