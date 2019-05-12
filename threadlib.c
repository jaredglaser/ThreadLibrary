#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
#include <signal.h>

tcb *runningQueue = NULL;
tcb *readyQueue = NULL;

int value = 0;

void t_init(){
  runningQueue = malloc(sizeof(tcb));
  runningQueue->value = malloc(sizeof(ucontext_t));
  runningQueue->thread_id = 0;
  getcontext(runningQueue->value);    // let back be the context of main() 
}
void t_shutdown(){
    tcb *temp = runningQueue;
    while(runningQueue !=NULL){
      temp = temp->next;
      free(runningQueue->value->uc_stack.ss_sp);
      free(runningQueue->value);
      free(runningQueue);
      runningQueue = temp;
    }
    temp = readyQueue;
    while(readyQueue !=NULL){
      temp = temp->next;
      free(readyQueue->value->uc_stack.ss_sp);
      free(readyQueue->value);
      free(readyQueue);
      readyQueue = temp;
    }
}
/*
* Put the current running process at the end of the queue 
*/
void t_yield()
{
  tcb *temp;

  if(readyQueue == NULL){ //there is only one thread that is yielding
    return;
  }
  temp = runningQueue; 
  runningQueue = readyQueue;
  if(readyQueue->next == NULL){
      readyQueue = NULL;
  }
  //readyQueue = readyQueue->next;
  //insert tmp to end of ready queue
  if(readyQueue == NULL){
      readyQueue = temp;
  }
  else{
      tcb *tmp = readyQueue;
  while(tmp->next !=NULL){
      // printf("id:%d \n",tmp->thread_id);
      tmp = tmp->next;
  }
    tmp->next = temp; //move runningQueue to the end of the ready queue
    tmp->next->next = NULL;
    readyQueue = readyQueue->next; //increment readyQueue
  }

  swapcontext(temp->value, runningQueue->value);
}

void t_terminate(){
    tcb *toDelete = runningQueue;
    runningQueue = readyQueue; //put first ready process as the running process
    readyQueue = readyQueue->next; //move the next ready process up in the queue
    free(toDelete->value->uc_stack.ss_sp);
    free(toDelete->value);
    free(toDelete);
    setcontext(runningQueue->value);
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
  if(readyQueue == NULL){
      readyQueue = newproc;
  }
  else{//add to the end of the queue
    tcb *temp = readyQueue;
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
  //sighold();
  sp->count--;
  tcb* temp = sp->q;
  if(runningQueue != NULL){
    runningQueue->next = NULL;
  }
  if(sp->count < 0){
    //add the current running tcb to the end of the semaphores queue
    if(sp->q == NULL){
      sp->q = runningQueue;
      sp->q->next = NULL;
      temp = runningQueue ;
    }
    else{
      while(temp->next != NULL){
        temp= temp->next;
      }
      temp->next = runningQueue;
      temp->next->next = NULL;
    }
    //put ready head as running head
    tcb* old = runningQueue;
    if(readyQueue != NULL){
    runningQueue = readyQueue;
    readyQueue = readyQueue->next;
    runningQueue->next = NULL;
    swapcontext(old->value,runningQueue->value);  
    }
  }
  //sigrelse();
}
void sem_signal(sem_t *sp){
  //sighold();
  sp->count++;
  if(sp->count <=0 && sp->q != NULL){
    tcb* temp = readyQueue;
    if(temp != NULL){ 
      //iterate through the ready queue
      while(temp->next != NULL){
        temp = temp->next;
      }
      //append on the first blocked thread
      //tcb* blocked = sp->q;
      temp->next = sp->q;
      temp->next->next = NULL;
      //iterate semephore queue
      sp->q = sp->q->next;
      //sigrelse();
    }
    else{ //ready Queue was null so we set sp->q as ready Queue
      readyQueue = sp->q;
      readyQueue->next = NULL;
      sp->q = sp->q->next;
    }
  }
  else{
  //sigrelse();
  }
}







void sem_destroy(sem_t **sp){
  //free(sp);
}
