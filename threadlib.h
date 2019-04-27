#include <ucontext.h>


typedef struct node
{ 
    ucontext_t *value;
    struct node *next;
    struct node *prev;
}node;
void t_init();
void t_yield();
void assign(long a, int *b);