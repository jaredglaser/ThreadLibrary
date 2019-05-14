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

void send(int tid, char *msg, int len); /* Send a message to the thread whose tid is tid. msg is the pointer to the start of the message, and len specifies the length of the message in bytes. In your implementation, all messages are character strings. */
void receive(int *tid, char *msg, int *len); /* Wait for and receive a message from another thread. The caller has to specify the sender's tid in tid, or sets tid to 0 if it intends to receive a message sent by any thread. If there is no "matching" message to receive, the calling thread waits (i.e., blocks itself). [A sending thread is responsible for waking up a waiting, receiving thread.] Upon returning, the message is stored starting at msg. The tid of the thread that sent the message is stored in tid, and the length of the message is stored in len. The caller of receive() is responsible for allocating the space in which the message is stored. Even if more than one message awaits the caller, only one message is returned per call to receive(). Messages are received in the order in which they were sent. The caller will not resume execution until it has received a message (blocking receive). */