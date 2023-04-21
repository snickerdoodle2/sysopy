#define MAX_CLIENTS 256
#define CLIENT_INIT 1

/*
 * msg_type = 1 - init klienta
 * */
struct init_msg {
	long msg_type;
	int IPC_ID;
};


