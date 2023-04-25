#define CLIENT_INIT 1

#define COMMAND_LIST 3
#define COMMAND_2ALL 4
#define COMMAND_2ONE 5
#define COMMAND_STOP 2

#define CLIENT_RESPONSE 6

#define SERVER_STOP 7



#define MAX_CLIENTS 256
#define RESPONSE_LENGTH 512

#define QUEUE_NAME "/server_queue"
#define MAX_MSG_SIZE 512
#define MAX_MSG_NUM 10


struct init_msg {
	long msg_type;
	char IPC_ID[64];
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
