typedef int Sema;

Sema create_sema(char * name, int amount, int initial_value);
Sema open_sema(char * name);

void close_sema(Sema sema_id);
void destroy_sema(char * name);

void increment(Sema sema_id, int room_id);
void decrement(Sema sema_id, int room_id);
void wait(Sema sema_id, int room_id);
