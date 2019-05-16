#ifndef H_PACKET
#define H_PACKET
typedef struct {
    char * data;
    int id;
} Packet;
void messageToClient(int id, char * m);
#endif
