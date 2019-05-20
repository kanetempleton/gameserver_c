#ifndef H_SERVER
#define H_SERVER
#include <netinet/in.h>
#define PORT 10000
#define BUFFER_SIZE 1025
typedef struct {
    
    int * randomData; //i dont need this right now or maybe ever

} Server;

Server * newServer();
void deleteServer(Server * me);
void initServer(Server * me, int maxclients);
void * startServer(void * arg);
void messageToClient(Server * me, int id, char * m);
void resetSendBuffer(Server * me);
#endif
