/* 
 * thread library function prototypes
 */
typedef void sem_t;  // for semaphore
typedef void mbox;   // for mailbox

int t_create(void (*func)(int), int thr_id, int pri);
void t_terminate();
void t_init();
void t_yield();
void assign(long a, int *b);
void t_shutdown();

int sem_init(sem_t **sp, int sem_count);
void sem_wait(sem_t *sp);
void sem_signal(sem_t *sp);
void sem_destroy(sem_t **sp);

int mbox_create(mbox **mb); /* Create a mailbox pointed to by mb. */
void mbox_destroy(mbox **mb); /* Destroy any state related to the mailbox pointed to by mb. */
void mbox_deposit(mbox *mb, char *msg, int len); /* Deposit message msg of length len into the mailbox pointed to by mb. */
void mbox_withdraw(mbox *mb, char *msg, int *len); /* Withdraw the first message from the mailbox pointed to by mb into msg and set the message's length in len accordingly*/