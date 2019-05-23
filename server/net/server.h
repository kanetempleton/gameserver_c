#ifndef H_SERVER
#define H_SERVER
#include <netinet/in.h>
#include "../game/game.h"
#define PORT 10000
#define BUFFER_SIZE 1025

typedef struct {
    char * name;
    int * id;
} Entry;

typedef struct {
    Entry* entries[100];
    int * currentSlot;
} EntryTable;

typedef struct {
    int * randomData; //i dont need this right now or maybe ever
    Game * game;
    EntryTable* userData;
} Server;

Server * newServer();
void deleteServer(Server * me);
void initServer(Server * me, int maxclients);
void * startServer(void * arg);
void messageToClient(int id, char * m);
void resetSendBuffer(Server * me);
char* getUsername(Server* me,int fd);
void addUsername(Server* me, int from, char* usr);
#endif
