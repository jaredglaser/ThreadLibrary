#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
#include <signal.h>

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
    tcb *temp = runningHead;
    while(runningHead !=NULL){
      temp = temp->next;
      free(runningHead->value->uc_stack.ss_sp);
      free(runningHead->value);
      free(runningHead);
      runningHead = temp;
    }
    temp = readyHead;
    while(readyHead !=NULL){
      temp = temp->next;
      free(readyHead->value->uc_stack.ss_sp);
      free(readyHead->value);
      free(readyHead);
      readyHead = temp;
    }
}
/*
* Put the current running process at the end of the queue 
*/
void t_yield()
{
  tcb *temp;

  if(readyHead == NULL){ //there is only one thread that is yielding
    return;
  }
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

  swapcontext(temp->value, runningHead->value);
}

void t_terminate(){
    tcb *toDelete = runningHead;
    runningHead = readyHead; //put first ready process as the running process
    readyHead = readyHead->next; //move the next ready process up in the queue
    free(toDelete->value->uc_stack.ss_sp);
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

int sem_init(sem_t **sp, int sem_count){
  *sp = malloc(sizeof(sem_t));
  (*sp)->count = sem_count;
  (*sp)->q = NULL;
}
void sem_wait(sem_t *sp){
  sighold();
  sp->count--;
  tcb* temp = sp->q;
  if(sp->count < 0){
    //add the current running tcb to the end of the semaphores queue
    if(sp->q == NULL){
      sp->q = runningHead;
      temp = runningHead ;
    }
    else{
      while(temp->next != NULL){
        temp= temp->next;
      }
      temp->next = runningHead;
    }
    //put ready head as running head
    tcb* old = runningHead;
    runningHead = readyHead;
    readyHead = readyHead->next;
    runningHead->next = NULL;
    swapcontext(old->value,runningHead->value);  
    
  }
  sigrelse();
}
void sem_signal(sem_t *sp){
  sighold();
  sp->count++;
  if(sp->count <=0 && sp->q != NULL){
    tcb* temp = readyHead;
    //iterate through the ready queue
    while(temp->next != NULL){
      temp = temp->next;
    }
    //append on the first blocked thread
    tcb* blocked = sp->q;
    temp->next = blocked;
    //iterate semephore queue
    sp->q = sp->q->next;
    sigrelse();
  }
  else{
  sigrelse();
  }
}
void sem_destroy(sem_t **sp){
  free(sp);
}
