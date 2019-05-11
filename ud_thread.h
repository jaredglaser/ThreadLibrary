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