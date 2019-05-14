#include <stdio.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "threadlib.h"
#include <stdlib.h>
#include <signal.h>

tcb *runningQueue = NULL;
tcb *readyQueue = NULL;

int value = 0;

void t_init()
{
  runningQueue = malloc(sizeof(tcb));
  runningQueue->value = malloc(sizeof(ucontext_t));
  runningQueue->thread_id = 0;
  getcontext(runningQueue->value); // let back be the context of main()
}
void t_shutdown()
{
  tcb *temp = runningQueue;
  while (runningQueue != NULL)
  {
    temp = temp->next;
    free(runningQueue->value->uc_stack.ss_sp);
    free(runningQueue->value);
    free(runningQueue);
    runningQueue = temp;
  }
  temp = readyQueue;
  while (readyQueue != NULL)
  {
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
  tcb *runningHead = runningQueue;
  tcb *temp = readyQueue;
  runningHead->next = NULL; //needs to be done or weird loops happen
  if (temp != NULL)
  {
    while (temp->next)
    {
      temp = temp->next;
    }
    temp->next = runningHead; //add current running thread to the end of the ready queue
    runningQueue = readyQueue; //make the running thread the head of the ready queue
    readyQueue = readyQueue->next; //iterate ready queue
    swapcontext(runningHead->value, runningQueue->value); //swap em
  }
}

void t_terminate()
{
  tcb *toDelete = runningQueue;
  runningQueue = readyQueue;     //put first ready process as the running process
  readyQueue = readyQueue->next; //move the next ready process up in the queue
  free(toDelete->value->uc_stack.ss_sp);
  free(toDelete->value);
  free(toDelete);
  setcontext(runningQueue->value);
}

int t_create(void (*func)(int), int thr_id, int pri)
{
  size_t sz = 0x10000;
  // char func_stack[16384];
  tcb *newproc = malloc(sizeof(tcb));
  newproc->value = malloc(sizeof(ucontext_t));
  getcontext(newproc->value);
  newproc->value->uc_stack.ss_sp = malloc(sz);
  ;
  newproc->value->uc_stack.ss_size = sz;
  newproc->value->uc_link = newproc->value;
  //set up thr_id and pri
  newproc->thread_id = thr_id;
  newproc->thread_priority = pri;
  makecontext(newproc->value, (void (*)(void))func, 1, thr_id);
  if (readyQueue == NULL)
  {
    readyQueue = newproc;
  }
  else
  { //add to the end of the queue
    tcb *temp = readyQueue;
    while (temp->next != NULL)
    {
      temp = temp->next;
    }
    temp->next = newproc;
    temp->next->next = NULL;
  }
}

int sem_init(sem_t **sp, int sem_count)
{
  *sp = malloc(sizeof(sem_t));
  (*sp)->count = sem_count;
  (*sp)->q = NULL;
}

void sem_wait(sem_t *sp)
{
  sighold();
  sp->count--;
  tcb *runningHead = runningQueue;

  if (runningHead != NULL)
  {
    runningHead->next = NULL;
  }
  if (sp->count < 0)
  {
    if (sp->q == NULL)
    { //then give it an initial value of the current running thread
      sp->q = runningHead;
    }
    else
    { //then add the current running thread to the end of the semaphore queue
      tcb *temp = sp->q;
      while (temp->next)
      {
        temp = temp->next;
      }
      temp->next = runningHead;
    }
    if(readyQueue !=NULL){
    runningQueue = readyQueue;     //put current running thread as ready thread
    
      readyQueue = readyQueue->next; //then iterate the ready queue
    swapcontext(runningHead->value, runningQueue->value);
    }
  }
  sigrelse();
}

void sem_signal(sem_t *sp)
{
  sighold();
  sp->count++;
  if (sp->count <= 0)
  {
    tcb *sem = sp->q;
    sp->q = sp->q->next; //iterate the head of the semaphore queue since we saved it in sem
    if (sem != NULL)
    {
      sem->next = NULL;
    }
    tcb *temp = readyQueue;
    if(temp!=NULL){
    while (temp->next) 
    {
      temp = temp->next;
    }
    temp->next = sem; //add the head of the semaphore queue to the end of the ready queue
    }
    else{
      readyQueue = sem;
    }
    sigrelse();
  }
}

void sem_destroy(sem_t **sp)
{
  free(*sp);
}

/* 
*Create a mailbox pointed to by mb.
 */
int mbox_create(mbox **mb){

} 
/* 
* Destroy any state related to the mailbox pointed to by mb.
 */
void mbox_destroy(mbox **mb){

}
/* 
* Deposit message msg of length len into the mailbox pointed to by mb. 
*/
void mbox_deposit(mbox *mb, char *msg, int len){

}
 /* 
 * Withdraw the first message from the mailbox pointed to by mb into msg and set the message's length in len accordingly
 */
 void mbox_withdraw(mbox *mb, char *msg, int *len){
   
 }