#include <ucontext.h>

struct tcb
{
  int thread_id;
  int thread_priority;
  ucontext_t *value;
  struct tcb *next;
};
typedef struct tcb tcb;

struct sem_t
{
  int count;
  tcb *q;
};
typedef struct sem_t sem_t;

struct messageNode
{
  char *message;            // copy of the message
  int len;                  // length of the message
  int sender;               // TID of sender thread
  int receiver;             // TID of receiver thread
  struct messageNode *next; // pointer to next node
};

typedef struct
{
  struct messageNode *msg; // message queue
  sem_t *mbox_sem;
} mbox;
