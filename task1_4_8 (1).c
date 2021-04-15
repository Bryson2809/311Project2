/** @file task1.c
* Demonstrates semaphores to contol the synchronization of threads.
*
* This program starts a number of processes that are synchronized with
* semaphores to control the text output.
*
* @author Robert van Engelen
* Modified 3/1/21 with permission by CAC
**/

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#ifdef _POSIX_THREADS
# include <pthread.h>
#endif

#include <semaphore.h>

void *text(void *arg);
int counter = 0; 

int code[] = { 0,1,2,3,4,5,6 };

sem_t sema0, sema1, sema2, sema3, sema4, sema5, sema6;

int main()
{
  int i;
  pthread_t tid[7];

  for (i = 0; i < 7; i++)
    pthread_create(&tid[i], NULL, text, (void*)&code[i]);

  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  pthread_join(tid[3], NULL);
  pthread_join(tid[4], NULL);
  pthread_join(tid[5], NULL);
  pthread_join(tid[6], NULL);

  return 0;
}

void *text(void *arg)
{
	int n = *(int*)arg;

    while (counter != n)
    {
        if (n != counter)
        {
            if (n == 0)
                sem_wait(&sema0);
            if (n == 1)
                sem_wait(&sema1);
            if (n == 2)
                sem_wait(&sema2);
            if (n == 3)
                sem_wait(&sema3);
            if (n == 4)
                sem_wait(&sema4);
            if (n == 5)
                sem_wait(&sema5);
            if (n == 6)
                sem_wait(&sema6);
        }
    }
  
  switch (n)
  {
    case 0:
      printf("A semaphore S is an integer-valued variable which can take only non-negative\n");
      printf("values. Exactly two operations are defined on a semaphore:\n\n");
      sem_post(&sema1);
      counter++;
      break;

    case 1:
      printf("Signal(S): If there are processes that have been suspended on this semaphore,\n");
      printf(" wake one of them, else S := S+1.\n\n");
      sem_post(&sema2);
      counter++;
      break;

    case 2:
      printf("Wait(S): If S>0 then S:=S-1, else suspend the execution of this process.\n");
      printf(" The process is said to be suspended on the semaphore S.\n\n");
      sem_post(&sema3);
      counter++;
      break;

    case 3:
      printf("The semaphore has the following properties:\n\n");
      sem_post(&sema4);
      counter++;
      break;

    case 4:
      printf("1. Signal(S) and Wait(S) are atomic instructions. In particular, no\n");
      printf(" instructions can be interleaved between the test that S>0 and the\n");
      printf(" decrement of S or the suspension of the calling process.\n\n");
      sem_post(&sema5);
      counter++;
      break;

    case 5:
      printf("2. A semaphore must be given an non-negative initial value.\n\n");
      sem_post(&sema6);
      counter++;
      break;

    case 6:
      printf("3. The Signal(S) operation must waken one of the suspended processes. The\n");
      printf(" definition does not specify which process will be awakened.\n\n");
      sem_post(&sema0);
      counter++;
      break;
  }

  pthread_exit(0);
}

