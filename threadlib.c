#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
tcb *runningHead;
tcb *readyHead;
char func_stack[16384];
int value = 0;

void t_init(){
  runningHead = malloc(sizeof(tcb));
  runningHead->value = malloc(sizeof(ucontext_t));
  readyHead = malloc(sizeof(tcb));
  readyHead->value = malloc(sizeof(ucontext_t));

  
  
}
int main(int argc, char **argv) 
{
  t_init();

  getcontext(runningHead->value);    /* let back be the context of main() */
   

  getcontext(readyHead->value);

  readyHead->value->uc_stack.ss_sp = func_stack;
  readyHead->value->uc_stack.ss_size = sizeof(func_stack);

  readyHead->value->uc_link = runningHead->value; 

  makecontext(readyHead->value, (void (*)(void)) assign, 2, 107L, &value);
  

  printf("in main(): 0\n");

  t_yield();

  printf("in main(): 1\n");
  t_yield();

  printf("in main(): 2\n");
  t_yield();

  printf("done %d\n", value);

  return (0);
}
/*
* Put the current running process at the end of the queue 
*/
void t_yield()
{
  tcb *temp;

  temp = runningHead; 
  runningHead = readyHead;
  readyHead = readyHead->next;
  //insert tmp to end of ready queue
  if(readyHead == NULL){
      readyHead = temp;
  }
  else{
      tcb *tmp = readyHead;
  while(tmp->next !=NULL){
      tmp = tmp->next;
  }
    tmp->next = temp; //move runningHead to the end of the ready queue
  }
  swapcontext(readyHead->value, runningHead->value);
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