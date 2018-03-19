#include "log.h"


void logger();
char * getDateTime(char * tmp);

main(int argc, char *argv[])
{
	strcpy(fileName,argv[1]);
	initialize();
	logger();

}

void logger()
{
	int len;
	for(;;) {
		memset(logBuf,0, sizeof(logBuf));
		if((len = leFila(LOGQ, logBuf, sizeof(logBuf)))== -1) {
			continue;
		}
		else {
			writeFile(logBuf);
		}
	}

}


int initialize()
{
	char tmp[80] = {0,};
	int result = SUCESS;
	if (!openFile())
	{
		printf("%s Problema ao abrir arquivo \n",getDateTime(tmp));
		result = FAILURE;
	}
	return result;
}


char * getDateTime(char * tmp)
{

	time_t in_tm;
	struct tm * ltime;

	time(&in_tm);
	ltime = localtime(&in_tm);

	sprintf(tmp, "[%04d/%02d/%02d %02d:%02d:%02d] - ",
	        ltime->tm_year + 1900,
	        ltime->tm_mon + 1,
	        ltime->tm_mday,
	        ltime->tm_hour,
	        ltime->tm_min,
	        ltime->tm_sec);

	return tmp;
}


void cleanup()
{
	closeFile();
}

int writeFile(char* data)
{
	char tmp[80] = {0,};
	bytesrd = strlen(data);
	int result = SUCESS;
	if (!fout) {

		if (!openFile())
		{
			result = FAILURE;
		}
	}
	if(result == SUCESS) {

		byteswr = write(fout, data, bytesrd);
		if (byteswr < bytesrd) {
			printf("Error writing  %s\n",  strerror(errno));
			exit(1);
		}

	}
	return result;
}

char *getDirLog(char *dir )
{
	char tmp[80] = {0,};
	
	int dirFd;

	memset(dir,'\0',(strlen(LOG_BASEPATH)+1));
	memcpy(dir,LOG_BASEPATH,strlen(LOG_BASEPATH));

	if ((dirFd = opendir(dir)) == 0)
	{
		if ((mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO) < 0))
		{
			printf ("%s  Erro criando diretorio -\n", getDateTime(tmp));
		}
		closedir(dirFd);
	}

	return dir;
}



int openFile()
{
	char dir[(strlen(LOG_BASEPATH)+1)];
	char tmp[80] = {0,};
	memset(pathFile, '\0' , sizeof(fileName));
	strcpy(pathFile , getDirLog(dir));
	strcat(pathFile ,"/");
	strcat(pathFile , fileName);

	fout  = creat(pathFile, 0660);
	if (fout < 0) {
		printf("Error creating '%s': %s\n", pathFile, strerror(errno));
		exit(1);
	}
	return SUCESS;
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
		printf("%02x", *p);
	}
	printf(">\n");
}


int closeFile()
{
	if (fout) { close(fout); }
	return SUCESS;
}

