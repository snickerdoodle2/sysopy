#define RES_ERROR 400
#define RES_SUCCESS 200
#define PACKET_SIZE 1024

struct response {
    int code;
    int packets;
};
