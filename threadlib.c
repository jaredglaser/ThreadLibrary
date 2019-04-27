#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
tcb *running;
tcb *ready;
char func_stack[16384];
int value = 0;

void t_init(){
  running = malloc(sizeof(tcb));
  running->value = malloc(sizeof(ucontext_t));
  ready = malloc(sizeof(tcb));
  ready->value = malloc(sizeof(ucontext_t));

  
  
}
int main(int argc, char **argv) 
{
  t_init();

  getcontext(running->value);    /* let back be the context of main() */
   

  getcontext(ready->value);

  ready->value->uc_stack.ss_sp = func_stack;
  ready->value->uc_stack.ss_size = sizeof(func_stack);

  ready->value->uc_link = running->value; 

  makecontext(ready->value, (void (*)(void)) assign, 2, 107L, &value);
  

  printf("in main(): 0\n");

  t_yield();

  printf("in main(): 1\n");
  t_yield();

  printf("in main(): 2\n");
  t_yield();

  printf("done %d\n", value);

  return (0);
}
void t_yield()
{
  struct node *tmp;

  tmp = running;
  running = ready;
  ready = tmp;

  swapcontext(ready->value, running->value);
}

void assign(long a, int *b)
{
  int i;

  *b = (int)a;

  for (i = 0; i < 3; i++)
    printf("in assign(1): %d\n", i);

  t_yield();

  for (i = 10; i < 13; i++)
    printf("in assign(2): %d\n", i);

  t_yield();

  for (i = 20; i < 23; i++)
    printf("in assign(3): %d\n", i);
}



/* --- output -----
in main(): 0
in assign(1): 0
in assign(1): 1
in assign(1): 2
in main(): 1
in assign(2): 10
in assign(2): 11
in assign(2): 12
in main(): 2
in assign(3): 20
in assign(3): 21
in assign(3): 22
done 107
*/