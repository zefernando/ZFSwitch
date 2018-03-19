
// fonte: proceda.d VERSAO DE DESENVOLVIMENTO, deverá executar somente na porta 32004
// fonte: proceda.d VERSAO DE DESENVOLVIMENTO, deverá executar somente na porta 32004
// fonte: proceda.d VERSAO DE DESENVOLVIMENTO, deverá executar somente na porta 32004
// fonte: proceda.d VERSAO DE DESENVOLVIMENTO, deverá executar somente na porta 32004

// Ailton 05/12/03

//----Programa elaborado por Ronaldo Arrudas 07/2003
//----
//----Update e Tunning: Renato Anderson de Lima
//----
//----Este programa recebe menagens do POS via Proceda
//----starta a funcao autorizadora do banco de dados e reponde p/ Proceda
//----
//----Portas: 32001 a 32004 (canais logicos)
//----
// Cabeçalhos gerais
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>

// Cabeçalhos personalizados
#include "con_tcp.h"
#include "authnbr.h"
#include "sqlcpr.h"
#include "sql_call.h"
#include "hex.h"


// Prototipos
int OpenConn( int port, int sd );
int  Receive( int sd, char *data );
void Answer( int sd, char *data, char *parm1, char *parm2 );
unsigned char Auth[6] = {0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0};
int soma = 0;
char buffer[393];
// Funcoes
int main( int argc, char *argv[] )
  {
  char parm1[39], parm2[159];
  char filler[13], tipoc;
  char estab1[5], estab2[5], nsu[13];
  char descparc[6], tpparc[2], numparc[3], plano[6];
  int conn, sd, port, br, tipoi, de3, para2;
  if( argc != 2){
    printf("\n\nSintaxe: %s <numero da porta (32001-32004)>\n\n\n", argv[0]);
    exit(-1);
    }
  port = atoi((char *)argv[1]);
  sd = tcp_connect( "192.168.1.1" , port );


  printf("\nProceda --- Versao 2\n");
  printf ( " \n SD  : %d ", sd);
  printf ( " \n port: %d ", port);

  if( sd == -1 ) {
    printf( "Fail to connect\n" );
    exit(1);
    }
    conn = OpenConn( port , sd );
  if (soma == 0)
   {
    soma = 1;

    // sql_connect("jardimsul", "jardimsul", "icreddb1");   /* Teste */
   //  sql_connect("alfa2", "alfa2", "icreddb1");   /* Teste */

  sql_connect("alfa", "banco", "cardp001");  /* Producao */
    printf ( " \n Conectou  \n");

   }

  if(conn == 0) {
    time_t rawtime;
   // pid_t p;
   // signal(SIGCLD, SIG_IGN);  /* para nao ter que esperar o processo filho wait()! */

    for(;;) {
      br = Receive( sd , buffer );
       if( br == -1 ) break;
      //retorna o horario de recebimento da mensagem

     // p = fork();
     // if (p==0){ //processo filho
     //  printf ( " \nEntra processo filho: %d \n", p);
        time ( &rawtime );
        printf ( " \nMensagem recebida as: %s \n", ctime (&rawtime) );

        hexdump( buffer , br );
// Montagem de parm1 e parm2 --------------------------------------------------
   /* Layout de parm1:
      AA..AABCCCCDDDDEFF..FF
      < 15 >1<04><04>1< 13 >
        A - Número do estabelecimento
        B - Tipo da solicitação
        C - Tipo da transação
        D - Instância
        E - Tipo do dispositivo
        F - Código do dispositivo

      Layout de parm2:
      AA..AABB..BBCC..CCDD..DDEE..EEFFGG..GGHHII..IIJJ..JJKK..KKLL..LL
      < 40 >< 28 >< 12 >< 12 >< 08 >02< 06 >02< 07 >< 08 >< 12 >
        A - Número do cartao (40)
        B - Senha (12)
        C - Valor (12)
        D - Entrada
        E - Vencimento
        F - Qtde. parcelas
        G - Fidelidade
        H - Tipo NF
        I - Número da NF
        J - Data ddmmaaaa
        K - Parâmetro mórbido
        L - numero do documento NSU (12)
 */

// Verifica se eh CANCELAMENTO (estorno)

        if(buffer[26] == 0x04)
        {
          tipoi = 9;
          tipoc = 'K';
          //  sprintf(filler, "000000%d%d%d%d%d%d");
          sprintf(filler, "000000000000");
          printf("Proceda: Estorno\n");
        }
        else if( buffer[26] == 0x01)
        {
          tipoi = 1;
          tipoc = 'K';
          sprintf(filler, "000000000000");
          printf("Proceda: Venda\n");
        }
//---
//--- estabelecimento recebido = bbbrrr0eeeellll ex. 025001005000502
//--- bbb  = banco = 025
//--- rrr  = rede = 001 RedeMais
//--- 0    = fixo zero, uso futuro
//--- eeee = estabelecimento = 0500
//--- llll = loja = 0502
//---
//--- no banco de dados, o codigo do estabelecimento eh 0000eeee99llllb
//--- 0000 = fixo zeros
//--- eeee = estabelecimento recebido na mensagem (0500)
//--- 99   = regional, hoje fixo 99
//--- llll = loja recebida na mensagem (0502)
//--- b    = branco (space)
//---
//--- Monta estab1 e estab2 para o parm1
//---
          para2 = 0;
          sprintf(estab1, "0000");
          for(de3 = 99; de3 <= 102; de3++ ) {
          estab1[para2] = buffer[de3] - 0xc0;
          para2++;
          }
          printf("Estab1\n");
          hexdump(estab1,5);
          para2 = 0;
          sprintf(estab2, "0000");
          for(de3 = 103; de3 <= 106; de3++ ) {
            estab2[para2] = buffer[de3] - 0xc0;
            para2++;
          }
          printf("Estab2\n");
          hexdump(estab2,5);

//--cartao = deslocamento 37 a 44 da mensagem recebida
//--valor  = deslocamento 48 a 53 da mensagem recebida
//--NSU    = deslocamento 72 a 83 da mensagem recebida, gerado no campo NSU
//---
//--- Monta NSU para o parm2
//---

          para2 = 0;
          sprintf(nsu, "000000000000");
          for(de3 = 72; de3 <= 83; de3++ ) {
              nsu[para2] = buffer[de3] - 0xc0;
              para2++;
              }
              printf("Nsu\n");
              hexdump(nsu,13);

          //---
          //--- Monta dados de parcelado
          //---

          sprintf(descparc, "     ");
          sprintf(tpparc, "0");
          sprintf(numparc, "00");
          sprintf(plano, "00000");

          if(buffer[26] == 0x01) {

             para2 = 0;
             for(de3 = 107; de3 <= 112; de3++ ) {
                 descparc[para2] = buffer[de3] - 0xc0;
                 para2++;
                 }
                 printf("Desc Parc\n");
                 hexdump(descparc,6);

           //  if( descparc == "PARC=") {

             //-- tipo parcelado
                tpparc[0] = buffer[112] - 0xc0;
                printf("TP Parc\n");
                hexdump(tpparc,2);

             //-- numero de parcelas
                numparc[0] = buffer[113] - 0xc0;
                numparc[1] = buffer[114] - 0xc0;
                printf("Num Parc\n");
                hexdump(numparc,3);

             //-- Plano
                para2 = 0;
                for(de3 = 129; de3 <= 132; de3++ ) {
                 plano[para2] = buffer[de3] - 0xc0;
                 para2++;
                 }
                printf("Plano\n");
                hexdump(plano,6);

                //---- Teste de Delay no recebimento---//
                /*printf("\nSistema Paralisado por 20 segundos...\n");
                sleep(20);
                time ( &rawtime );
                printf ( " \nReiniciando processamento as: %s \n", ctime (&rawtime) );
                */
                //----- Fim teste ------------//
               }
             //}


          //--- sprintf(parm1, "00000500990502 %d01010000%c1234567890123", tipoi, tipoc);
                sprintf(parm1, "0000%s99%s %d01010000%c1234567890123", estab1, estab2, tipoi, tipoc);
                sprintf(parm2, "00%.2X%.2X%.2X0%.2X%.2X%.2X%.2X%.2X                                                 %.2X%.2X%.2X%.2X%.2x%.2X0000000000000000000001000000CF000000006082003%s%s%s%s%s",
                  (unsigned char)buffer[37],(unsigned char)buffer[38],
                  (unsigned char)buffer[39],(unsigned char)buffer[40],
                  (unsigned char)buffer[41],(unsigned char)buffer[42],
                  (unsigned char)buffer[43],(unsigned char)buffer[44],
                  (unsigned char)buffer[48],(unsigned char)buffer[49],
                  (unsigned char)buffer[50],(unsigned char)buffer[51],
                  (unsigned char)buffer[52],(unsigned char)buffer[53],
                  filler, nsu, tpparc, numparc, plano);
          //-----------------------------------------------------------------------------
                //Se operacao for 400 muda conteudo do parm2 pra chamar funcao no banco
                 if(buffer[26] == 0x04){
                   printf("Proceda: Reversão\n");
                   //monta NSU
                   para2 = 0;
                   sprintf(nsu, "000000000000");
                   if (buffer[29] == 0x04)
                      {
                      for(de3 = 80; de3 <= 91; de3++ ){
                         nsu[para2] = buffer[de3] - 0xc0;
                         para2++;
                          }
                      }
                   else if (buffer[29] == 0x00)
                        {
                        if  (buffer[44] == 0x13)
                          {
                          for(de3 = 80; de3 <= 91; de3++ ){
                             nsu[para2] = buffer[de3] - 0xc0;
                             para2++;
                             }
                          }
                        else if  (buffer[44] == 0x10)
                            {
                            for(de3 = 78; de3 <= 89; de3++ ){
                            nsu[para2] = buffer[de3] - 0xc0;
                            para2++;
                            }
                            }
                        }
                   printf("Nsu\n");
                   hexdump(nsu,13);

                   if (buffer[44] == 0x13)
                   {
                   sprintf(parm2, "00%.2X%.2X%.2X0%.2X%.2X%.2X%.2X%.2X                                                 %.2X%.2X%.2X%.2X%.2x%.2X0000000000000000000001000000CF000000006082003%s%s%s%s%s",
                 //  sprintf(parm2, "00%.2X%.2X%.2X0%.2X%.2X%.2X%.2X%.2X                                                 %s0000000000000000000001000000CF000000006082003%s%s%s%s%s",
                       (unsigned char)buffer[45],(unsigned char)buffer[46],
                       (unsigned char)buffer[47],(unsigned char)buffer[48],
                       (unsigned char)buffer[49],(unsigned char)buffer[50],
                       (unsigned char)buffer[51],(unsigned char)buffer[52],
                       //(unsigned char)buffer[56],(unsigned char)buffer[57],
                       (unsigned char)buffer[58],(unsigned char)buffer[59],
                       (unsigned char)buffer[60],(unsigned char)buffer[61],
                       (unsigned char)buffer[62],(unsigned char)buffer[63],

                          filler, nsu, tpparc, numparc, plano);
                   }
                   else if (buffer[44] == 0x10)
                    {
                   sprintf(parm2, "00%.2X%.2X%.2X0%.2X%.2X%.2X%.2X%.2X                                                 %.2X%.2X%.2X%.2X%.2x%.2X0000000000000000000001000000CF000000006082003%s%s%s%s%s",
                 //  sprintf(parm2, "00%.2X%.2X%.2X0%.2X%.2X%.2X%.2X%.2X                                                 %s0000000000000000000001000000CF000000006082003%s%s%s%s%s",
                       (unsigned char)buffer[45],(unsigned char)buffer[46],
                       (unsigned char)buffer[47],(unsigned char)buffer[48],
                       (unsigned char)buffer[49],(unsigned char)buffer[50],
                       (unsigned char)buffer[51],(unsigned char)buffer[52],
                       (unsigned char)buffer[56],(unsigned char)buffer[57],
                       (unsigned char)buffer[58],(unsigned char)buffer[59],
                       (unsigned char)buffer[60],(unsigned char)buffer[61],
                       //(unsigned char)buffer[62],(unsigned char)buffer[63],

                          filler, nsu, tpparc, numparc, plano);
                   }



                 }

          Answer( sd, buffer, parm1, parm2);
      //    printf ( " \nSai processo filho: %d \n", p);
      //    exit(0 ); //sai do processo filho
      //  }
      }

    }
  printf("###########################################################\n");
  printf("Proceda: ATENCAO: PROBLEMAS NA CONEXÃO X.25 - VERIFICAR\n");
  printf("###########################################################\n");
  close(sd);
  }

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

int Receive( int sd , char *data )
  {
  char f_buff[1024];
  int s,ptr;
  unsigned char header[2];
  printf("___________________________________________________________\n");
  printf("Proceda teste/desenvolvimento: Aguardando transacao.\n");
  so_read( sd , (char *)header , 2 );
  printf( "Header : %.2x %.2x\n" , header[0] , header[1] );
  if( header[0] == 0x40 ) {
    s = (int)header[1];
    printf( "Mensagem de comando (bytes): %d\n" , s );
    so_read( sd , f_buff , s );
    hexdump( f_buff , s);
    return(-1);
    }
  ptr = 0;
  while( header[0] == 0x20 ) {
    s = (int)header[1];
    printf( "Fragmento da mensagem (bytes): %d\n" , s );
    so_read( sd , &data[ptr] , s );
    ptr += s;
    so_read( sd , (char *)header , 2 );
    printf( "Header : %.2x %.2x\n" , header[0] , header[1] );
    }
  if( header[0] == 0 ) {
    s = (int)header[1];
    printf( "Ultimo fragmento da mensagem (bytes): %d\n" , s );
    so_read( sd , &data[ptr] , s );
    ptr += s;
    return(ptr);
    }
   return(-1);
  }

void Answer( int sd, char *data, char *parm1, char *parm2 )
  {
  printf("Proceda: Preparando resposta...\n");
  int de1, de2, para, Retorno;
  char dado[393], *Resposta;
  time_t rawtime;
// Chamada ao banco -----------------------------------------------------------
  printf("\tIniciando consulta ao banco.\n");
  printf("Parm1\n");
  hexdump(parm1, 39);
  printf("Parm2\n");
  hexdump(parm2, 159);


  //sql_connect("alfa", "alfa", "icreddb1");   /* Teste */
  //sql_connect("jardimsul", "jardimsul", "icreddb1");  /* jardimsul - teste */
  //sql_connect("alfa2", "alfa2", "icreddb1");  /* teste */

/************************* Inicio do processo de  reversao **********************************/
if(buffer[26] == 0x04){ // cancelamento estah sendo respondido sempre erro
    printf("\nReversao1\n");
    //Resposta[0] = 0;
    memset(Resposta, '1', 512);
    Resposta = sql_valida_trans(parm1,parm2,&Retorno);
    //memset(Resposta, '1', 392);
    //sprintf (Resposta, "19999                                      ");
    printf("Retorno1:%d\nResposta:\n", Retorno);
    hexdump(Resposta, 393);
    //hexdump(Resposta, 512);
    printf("\tConsulta1 concluida. Respondendo1.\n");
    // Construcao do pacote de resposta -------------------------------------------
    de1 = 0;
    de2 = 0;
    para = 2;
    dado[0] = 0x00;
    dado[1] = 0xa0;
    printf("\tResposta1: Transacao ");
    for(de1 = 0x00; de1 <= 0x1C ; de1++ ){
      dado[para] = data[de1];
      para++;
    }

    dado[29] = 0x10;

    dado[30] = 0xF2;
    dado[31] = 0x00;
    dado[32] = 0x00;
    dado[33] = 0x01;
    dado[34] = 0x0A;
    dado[35] = 0xC0;
    dado[36] = 0x80;
    dado[37] = 0x00;
    dado[38] = 0x00;
    dado[39] = 0x00;
    dado[40] = 0x00;
    dado[41] = 0x40;
    dado[42] = 0x00;
    dado[43] = 0x00;
    dado[44] = 0x00;
    dado[45] = 0x00;



    if (data[44] == 0x13)
       {
        // cartao , processing code , valor e data hora
        //dado[46] = data[44]
        // posicao de1 - 44 ate 68

        para = 46;
        for(de1 = 0x2C; de1 <= 0x44 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        for(de1 = 0x4C; de1 <= 0x5B ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        //codigo resposta

       if(strncmp(&Resposta[0],"0",1)==0)
           {
           printf("nova Reversão OK\n");
           dado[para] = 0xF0;
           para++;
           dado[para] = 0xF0;
           para++;
           }
       else
         {
         if(strncmp(&Resposta[4],"0089",4)==0 || strncmp(&Resposta[4],"0031",4)==0){
             dado[para] = 0xF9;
             para++;
             dado[para] = 0xF6;
             para++;
            }
         else
           {
           if(strncmp(&Resposta[4],"0032",4)==0)
             {
             dado[para] = 0xF7;
             para++;
             dado[para] = 0xF6;
             para++;
             }
           }
         }


        // Estabelecimento,loja,de 49

        for(de1 = 0x62; de1 <= 0x7A ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        for(de1 = 0x7D; de1 <= 0x91 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        dado[para] = 0x00;

        printf("\n Posicao 0 Resposta: %s", &Resposta[0]);
        printf("\n Posicao 1 Resposta: %s", &Resposta[1]);
        printf("\nMensagem de resposta:\n");
//        hexdump((char *)dado , 136 );
//        printf("\nSistema Paralisado por 30 segundos...\n");
//        sleep(30);
//        time ( &rawtime );
//        printf ( " \nReiniciando processamento as: %s \n", ctime (&rawtime) );
//        write( sd , dado , 136 );
        hexdump((char *)dado , 162 );
        write( sd , dado , 162 );

       }
    else
       {
       if (data[44] == 0x10)
       {
        // cartao , processing code , valor e data hora
        //dado[46] = data[44]
        // posicao de1 - 44 ate 68

        para = 46;
        for(de1 = 0x2C; de1 <= 0x42 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        for(de1 = 0x4A; de1 <= 0x59 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }
        //codigo resposta

       if(strncmp(&Resposta[0],"0",1)==0)
           {
           printf(" nova Reversão OK\n");
           dado[para] = 0xF0;
           para++;
           dado[para] = 0xF0;
           para++;
           }
       else
         {
         if(strncmp(&Resposta[4],"0089",4)==0 || strncmp(&Resposta[4],"0031",4)==0){
             printf("Erro na Reversao: 96");
             dado[para] = 0xF9;
             para++;
             dado[para] = 0xF6;
             para++;
            }
         else
           {
           if(strncmp(&Resposta[4],"0032",4)==0)
             {
             printf("Erro na Reversao: 76");
             dado[para] = 0xF7;
             para++;
             dado[para] = 0xF6;
             para++;
             }
             else
             {
             if(strncmp(&Resposta[0],"9",1)==0){
                Resposta[0] = 0;
                printf("Erro Interno na reversao...\n");
                dado[para] = 0xF9;
                para++;
                dado[para] = 0xF6;
                para++;
                }
             }
           }
         }

        // Estabelecimento,loja,de 49

        for(de1 = 0x60; de1 <= 0x78 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }

        for(de1 = 0x7B; de1 <= 0x89 ; de1++ )
        {
        dado[para] = data[de1];
        para++;
        }
        dado[para] = 0x00;

        printf("\n Posicao 0 Resposta: %s", &Resposta[0]);
        printf("\n Posicao 1 Resposta: %s", &Resposta[1]);
        printf("\nMensagem de resposta:\n");
//        hexdump((char *)dado , 134 );
//        printf("\nSistema Paralisado por 30 segundos...\n");
//        sleep(30);
//        time ( &rawtime );
//        printf ( " \nReiniciando 1 processamento as: %s \n", ctime (&rawtime) );
//
//        write( sd , dado , 134 );
          hexdump((char *)dado , 162 );
          write( sd , dado , 162 );

        }
       }


    // * OK



/************************* fim do processo de  reversao **********************************/

   }else{
   printf("\nORACLE a vista.\n");
   Resposta = sql_valida_trans(parm1,parm2,&Retorno);
   }
if(buffer[26] == 0x01)
  {
  printf("Retorno:%d\nResposta:\n", Retorno);
  hexdump(Resposta, 393);
  printf("\tConsulta concluida. Respondendo.\n");
  }
// Construcao do pacote de resposta -------------------------------------------
if(buffer[26] == 0x01)
  {
  de1 = 0;
  de2 = 0;
  para = 2;
  dado[0] = 0x00;
  dado[1] = 0xa0;
  printf("\tResposta: Transacao ");
  for(de1 = 0x00; de1 <= 0x53 ; de1++ ) {
    dado[para] = data[de1];
    para++;
    }
  dado[29] = 0x10;
  // * APROVADA
  if(strncmp(&Resposta[0],"0",1)==0)
   {
    printf("aprovada.\n");
    dado[34] = 0x0E;

    for(de2 = 43; de2 <= 48; de2++) {
      dado[para] = Resposta[de2] + 0xc0; // pega o codigo de autorizacao
      para++;
      }
    dado[para] = 0xf0;
    para++;
    dado[para] = 0xf0;
    para++;
    }
   else {
    // * REPROVADA...
    if(strncmp(&Resposta[0],"1",1)==0) {
      printf("reprovada ");
      dado[34] = 0x0a;
      // * ...TRANS. INVALIDA
      if(strncmp(&Resposta[4],"0011",4)==0 ||
         strncmp(&Resposta[4],"0020",4)==0 ||
         strncmp(&Resposta[4],"0051",4)==0 ||
         strncmp(&Resposta[4],"0055",4)==0 ||
         strncmp(&Resposta[4],"0062",4)==0 ||
         strncmp(&Resposta[4],"0091",4)==0 ||
         strncmp(&Resposta[4],"0094",4)==0 ||
         strncmp(&Resposta[4],"0095",4)==0 ||
         strncmp(&Resposta[4],"0156",4)==0) {
        printf("(Transação inválida).\n");
        dado[para] = 0xf1;
        para++;
        dado[para] = 0xf2;
        para++;
        }
       else {
        // ...CARTAO INVALIDO
        if(strncmp(&Resposta[4],"0001",4)==0 ||
           strncmp(&Resposta[4],"0002",4)==0 ||
           strncmp(&Resposta[4],"0005",4)==0 ||
           strncmp(&Resposta[4],"0010",4)==0 ||
           strncmp(&Resposta[4],"0013",4)==0 ||
           strncmp(&Resposta[4],"9301",4)==0 ||
           strncmp(&Resposta[4],"9302",4)==0 ||
           strncmp(&Resposta[4],"9303",4)==0 ||
           strncmp(&Resposta[4],"9304",4)==0 ||
           strncmp(&Resposta[4],"9305",4)==0 ||
           strncmp(&Resposta[4],"9410",4)==0) {
          printf("(Cartão inválido).\n");
          dado[para] = 0xf1;
          para++;
          dado[para] = 0xf4;
          para++;
          }
         else {
          // ...ESTAB. INVALIDO
          if(strncmp(&Resposta[4],"0006",4)==0 ||
             strncmp(&Resposta[4],"0015",4)==0 ||
             strncmp(&Resposta[4],"0045",4)==0) {
            printf("(Estabelecimento inválido).\n");
            dado[para] = 0xf0;
            para++;
            dado[para] = 0xf3;
            para++;
            }
           else {
            // ...CARTAO BLOQUEADO
            if(strncmp(&Resposta[4],"0004",4)==0 ||
               strncmp(&Resposta[4],"0007",4)==0 ||
               strncmp(&Resposta[4],"0017",4)==0 ||
               strncmp(&Resposta[4],"0018",4)==0 ||
               strncmp(&Resposta[4],"0023",4)==0 ||
               strncmp(&Resposta[4],"0024",4)==0 ||
               strncmp(&Resposta[4],"0058",4)==0 ||
               strncmp(&Resposta[4],"9280",4)==0) {
              printf("(Trans. não permitida para o cartão).\n");
              dado[para] = 0xf5;
              para++;
              dado[para] = 0xf7;
              para++;
              }
             else {
              // ...Insuficiencia de fundos
              if(strncmp(&Resposta[4],"0008",4)==0 ||
                 strncmp(&Resposta[4],"0019",4)==0 ||
                 strncmp(&Resposta[4],"0027",4)==0 ||
                 strncmp(&Resposta[4],"0057",4)==0) {
                printf("(Insuficiência de fundos).\n");
                dado[para] = 0xf5;
                para++;
                dado[para] = 0xf1;
                para++;
                }
               else {
                // ...SENHA INVALIDA
                if(strncmp(&Resposta[4],"0003",4)==0) {
                  printf("(Senha não confere).\n");
                  dado[para] = 0xf5;
                  para++;
                  dado[para] = 0xf5;
                  para++;
                  }
                 else {
                  // ...CARTAO VENCIDO
                  if(strncmp(&Resposta[4],"0016",4)==0) {
                    printf("(Cartão vencido).\n");
                    dado[para] = 0xf5;
                    para++;
                    dado[para] = 0xf4;
                    para++;
                    }
                   else {
                    // ...CARTAO PERDIDO
                    if(strncmp(&Resposta[4],"9204",4)==0) {
                      printf("(Cartão perdido).\n");
                      dado[para] = 0xf4;
                      para++;
                      dado[para] = 0xf1;
                      para++;
                      }
                     else {
                      // ...CARTAO ROUBADO
                      if(strncmp(&Resposta[4],"9205",4)==0) {
                        printf("(Cartão roubado).\n");
                        dado[para] = 0xf4;
                        para++;
                        dado[para] = 0xf3;
                        para++;
                        }
                       else {
                         // ...CANCELAMENTO erro = 11111111
                         if(strncmp(&Resposta[0],"11111111",8)==0) {
                           printf("(Cancelamento nao encontrado).\n");
                           dado[para] = 0xf2;
                           para++;
                           dado[para] = 0xf1;
                           para++;
                           }

                        else {
                           // ...Outras recusas
                           printf("(Outros).\n");
                           dado[para] = 0xf0;
                           para++;
                           dado[para] = 0xf5;
                           para++;
                           }
}}}}}}}}}}
     else {
      // ERRO INTERNO => REPROVADA
        if(strncmp(&Resposta[0],"9",1)==0){
            printf("Erro Interno...\n");
            dado[34] = 0x0a;
            dado[para] = 0xf9;
            para++;
            dado[para] = 0xf6;
            para++;
        }
      }
    }
}
if(buffer[26] == 0x01)
  {
  for(de1 = 0x54; de1 <= 0x97; de1++) {
    dado[para] = data[de1];
    para++;
    }
    printf("\nMensagem de resposta:\n");
    if(strncmp(&Resposta[0],"0",1)==0) {
       hexdump((char *)dado , 162 );
       write( sd , dado , 162 );
       }
    else
       {
       dado[157] = 0x00;
//       hexdump((char *)dado , 156 );
//       write( sd , dado , 156 );
        hexdump((char *)dado , 162 );
        write( sd , dado , 162 );

       }
  }
 /* printf("\nMensagem de resposta:\n");
  hexdump((char *)dado , 162 );
  write( sd , dado , 162 ); */

  time ( &rawtime );
  printf ( " \nMensagem respondida as: %s \n", ctime (&rawtime) );
  printf("\n Dado de Retorno: %s  Tam: %d\n", dado,strlen(dado));
  printf("Proceda: Transacao concluida.\n");
  printf("___________________________________________________________\n");
  }
