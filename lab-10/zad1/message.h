#define MSG_LEN 128

#define MSG_INIT 0
#define MSG_2ALL 1
#define MSG_2ONE 2
#define MSG_LIST 3
#define MSG_STOP 4
#define MSG_FULL 5

struct message {
    int type;
    char msg_body[MSG_LEN];
    char recipient_nickname[MSG_LEN];
};
