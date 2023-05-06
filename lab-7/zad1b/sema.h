#include <semaphore.h>

typedef sem_t ** Sema;

Sema create_sema(char * name, int amount, int initial_value);
Sema open_sema(char * name, int amount);

void close_sema(Sema sema_id, int amount);
void destroy_sema(char * name, int amount);

void increment(Sema sema_id, int room_id);
void wait_sema(Sema sema_id, int room_id);
