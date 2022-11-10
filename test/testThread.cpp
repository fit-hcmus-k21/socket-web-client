// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
// #include <pthread.h>

// #pragma comment (lib, "pthreadVC2.lib")

// // A normal C function that is executed as a thread
// // when its name is specified in pthread_create()
// void *myThreadFun(void *vargp)
// {
//     sleep(1);
//     printf("Printing GeeksQuiz from Thread \n");
//     return NULL;
// }
  
// int main()
// {
//     pthread_t thread_id;
//     printf("Before Thread\n");
//     pthread_create(&thread_id, NULL, myThreadFun, NULL);
//     pthread_join(thread_id, NULL);
//     printf("After Thread\n");
//     exit(0);
// }

#include <stdio.h>
#include "pthread.h"
#include "sched.h"

int main(int argc, char** argv)
{
  (void)argv;
  printf("######## start \n");
  return ((int*)(&pthread_create))[argc];
}