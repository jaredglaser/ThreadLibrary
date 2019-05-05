/* 
 * thread library function prototypes
 */
int t_create(void (*func)(int), int thr_id, int pri);
void t_terminate();
void t_init();
void t_yield();
void assign(long a, int *b);
void t_shutdown();