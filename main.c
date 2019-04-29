
#include <stdio.h>
#include "threadlib.h"

void function(int thr_id) 
{
    ucontext_t temp;
   getcontext(&temp);
   printf("CURRENT:%p\n",&temp);

   int i, j; 

   for (i = j = 0; i < 3; i++, j++) {
      printf("this is thread %d [%d]...\n", thr_id, j);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

int main(void)
{
   int i;

   t_init();
   t_create(function, 1, 1);
   printf("This is main(1)...\n");
   t_create(function, 2, 1);
   printf("This is main(2)...\n");
   t_create(function, 3, 1);

   for (i = 0; i < 4; i++) {
      printf("This is main(3)[%d]...\n", i);
      t_yield();
   }

   printf("Begin shutdown...\n");
   t_shutdown();
   printf("Done with shutdown...\n");

   return 0;
}

/*int main(int argc, char **argv) 
{
  t_init();

  getcontext(runningHead->value);    // let back be the context of main() 
   

  //getcontext(readyHead->value);

  //readyHead->value->uc_stack.ss_sp = func_stack;
  //readyHead->value->uc_stack.ss_size = sizeof(func_stack);

  //readyHead->value->uc_link = runningHead->value; 

  //makecontext(readyHead->value, (void (*)(void)) assign, 2, 107L, &value);
  t_create((void (*)(int))assign,1,1);

  printf("in main(): 0\n");

  t_yield();

  printf("in main(): 1\n");
  t_yield();

  printf("in main(): 2\n");
  t_yield();

  printf("done %d\n", value);

  return (0);
}
*/