#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
tcb *runningHead = NULL;
tcb *readyHead = NULL;

int value = 0;

void t_init(){
  runningHead = malloc(sizeof(tcb));
  runningHead->value = malloc(sizeof(ucontext_t));
  runningHead->thread_id = 0;
  getcontext(runningHead->value);    // let back be the context of main() 
}
void t_shutdown(){
    
}
/*
* Put the current running process at the end of the queue 
*/
void t_yield()
{
  tcb *temp;

  temp = runningHead; 
  runningHead = readyHead;
  if(readyHead->next == NULL){
      readyHead = NULL;
  }
  //readyHead = readyHead->next;
  //insert tmp to end of ready queue
  if(readyHead == NULL){
      readyHead = temp;
  }
  else{
      tcb *tmp = readyHead;
  while(tmp->next !=NULL){
      // printf("id:%d \n",tmp->thread_id);
      tmp = tmp->next;
  }
    tmp->next = temp; //move runningHead to the end of the ready queue
    tmp->next->next = NULL;
    readyHead = readyHead->next; //increment readyHead
  }

/*
  printf("running: %p\n",runningHead->value);
  tcb *a = readyHead;
  int i = 0;
  while(a != NULL){
  printf("ready:%p[%d]\n",a->value,i);
  i++;
  a = a->next;
  
  }
  */


  swapcontext(temp->value, runningHead->value);
}

void t_terminate(){
    tcb *toDelete = runningHead;
    runningHead = readyHead; //put first ready process as the running process
    readyHead = readyHead->next; //move the next ready process up in the queue
    free(toDelete->value);
    free(toDelete);
    setcontext(runningHead->value);
}

int t_create(void (*func)(int), int thr_id, int pri){
  size_t sz = 0x10000;
  // char func_stack[16384];
  tcb *newproc = malloc(sizeof(tcb));
  newproc->value = malloc(sizeof(ucontext_t));
  getcontext(newproc->value);
  newproc->value->uc_stack.ss_sp = malloc(sz);;
  newproc->value->uc_stack.ss_size = sz;
  newproc->value->uc_link = newproc->value; 
  //set up thr_id and pri
  newproc->thread_id = thr_id;
  newproc->thread_priority = pri;
  makecontext(newproc->value, (void (*)(void)) func,1, thr_id);
  if(readyHead == NULL){
      readyHead = newproc;
  }
  else{//add to the end of the queue
    tcb *temp = readyHead;
    while(temp->next != NULL){
        temp = temp -> next;
    }
    temp->next = newproc;
    temp->next->next = NULL;
  }

}
/*
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
*/


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
