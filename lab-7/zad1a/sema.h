#define CLIENT_EXE "./client"
#define BARBER_EXE "./barber"
#define CLIENT_AMOUNT 10;
#define BARBER_AMOUNT 6;
#define CHAIR_AMOUNT 5;
#define LOBBY_CAP 3;

typedef int Sema;

Sema create_sema(char * name, int amount, int initial_value);
Sema open_sema(char * name);

void close_sema(Sema sema_id);
void destroy_sema(char * name);

void increment(Sema sema_id, int room_id);
void decrement(Sema sema_id, int room_id);
void wait(Sema sema_id, int room_id);
