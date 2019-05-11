#include <ucontext.h>


struct tcb {
	  int thread_id;
    int thread_priority;
	  ucontext_t *value;
	  struct tcb *next;
    };
    typedef struct tcb tcb;

struct sem_t {
         int count;
         tcb *q;
       };
       typedef struct sem_t sem_t;
