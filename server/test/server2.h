#ifndef H_SERVER2
#define H_SERVER2
typedef struct {
    char * sendMessage;
    int * sendTo;
} Server2;
Server2* newServer2();
void initServer2(Server2* s);
void deleteServer2(Server2* s);
void * startServer2(void* arg);
void messageToClient2(Server2* s, int l, char* send);
#endif
