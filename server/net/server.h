#ifndef H_SERVER
#define H_SERVER
#include <netinet/in.h>
#define PORT 10000
#define BUFFER_SIZE 1025
typedef struct {
    struct sockaddr_in * address; //perhaps i shouldnt put all these here.. lol
    int * opt;
    int * master_socket;
    int * addrlen;
    int * new_socket;
    int * activity;
    int * i;
    int * valread;
    int * sd;
    int *max_sd;
    //int * client_socket[30];
    int * max_clients;
    //char * buffer[1025];
    //char * sendbuffer[1025];
    fd_set * readfds; //set of socket descriptors

    char * nextSendMsg;
    int * nextSendTo;

} Server;

Server * newServer();
void deleteServer(Server * me);
void initServer(Server * me, int maxclients);
void * startServer(void * arg);
void messageToClient(Server * me, int id, char * m);
void resetSendBuffer(Server * me);
#endif
