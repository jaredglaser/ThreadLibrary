#include <ucontext.h>


struct tcb {
	  int thread_id;
    int thread_priority;
	  ucontext_t *value;
	  struct tcb *next;
    };
    typedef struct tcb tcb;

void t_init();
void t_yield();
void assign(long a, int *b);