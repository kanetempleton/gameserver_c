#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "net/server.h"
#include <pthread.h>
#include <unistd.h>
#include "net/io/cmd.h"

int main(int argc, char* argv[]) {
    pthread_t p,p2;
    Server * server = newServer();
    CMD * cmd = newCMD();
    initServer(server,30);
    initCMD(cmd,server);
    printf("Attempting to create server thread from %d\n",getpid());
    int rc1 = pthread_create(&p,NULL,&startServer,(void*)server);
    int rc2 = pthread_create(&p2,NULL,&listenForCommands,(void*)cmd);
    void * rval1;
    void * rval2;
    rc1 = pthread_join(p, &rval1);
    rc2 = pthread_join(p2,&rval2);

}
