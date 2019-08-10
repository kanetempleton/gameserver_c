#ifndef H_SERVER
#define H_SERVER
#include <netinet/in.h>
#include "../game/game.h"
#define PORT 10000
#define BUFFER_SIZE 1025

typedef struct {

} Server;

Game* mainGame;
Server * newServer();
void deleteServer(Server * me);
void initServer(Server * me, int maxclients);
void * startServer(void * arg);
void messageToClient(int id, char * m);
void messageToClient_fd(int fd, char* m);
void kickClient(int fd);
void messageToAll(char* m, int ds);
#endif
