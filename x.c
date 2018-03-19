#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

void *ToNet();
void *FromNet();
void run();


extern int errno;
int sock = -1;
FILE *logfd;	
void TrataSinal(int n);

int main(int argc, char *argv[])
{
   run();

   while(1)
     sleep(2);
}



void run()
{
pthread_t fromThread;
pthread_t toThread;

		
 pthread_create(&fromThread, NULL, FromNet, NULL);

 pthread_create(&toThread, NULL, ToNet, NULL);
}

void *FromNet() {
  while(1)
    {

     printf("A pid = %d", getpid());

      fflush(stdout);
     sleep(1);
    }
}

void *ToNet() {
  while(1)
    {

     printf("B pid = %d", getpid());
      fflush(stdout);
     sleep(1);
    }
}
