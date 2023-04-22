#define MAX_CLIENTS 256
#define RESPONSE_LENGTH 512

#define CLIENT_INIT 1

#define COMMAND_LIST 2
#define COMMAND_2ALL 3
#define COMMAND_2ONE 4
#define COMMAND_STOP 5

#define CLIENT_RESPONSE 6

struct init_msg {
	long msg_type;
	int IPC_ID;
};

struct command_msg {
	long msg_type;
	int recipient_id;
	char msg[RESPONSE_LENGTH];
};

struct response_msg {
	long msg_type;
	char res[RESPONSE_LENGTH];
};