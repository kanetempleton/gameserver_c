#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "net/server.h"
#include <pthread.h>
#include <unistd.h>
#include "net/io/cmd.h"
#include "main.h"
#include "util/HashMap.h"

//Server* mainServer;

int main(int argc, char* argv[]) {
    pthread_t p,p2;
    Server*  mainServer = newServer();
    CMD * cmd = newCMD();
    initServer(mainServer,30);
    initCMD(cmd,mainServer);
    //printf("Attempting to create server thread from %d\n",getpid());
    /*printf("creating hash table...\n");
    HashMap* map = newHashMap();
    initHashMap(map);
    for (int i=0; i<25; i++) {
        hashMap_add(map,i*100,i+1);
    }
    printMap(map);
    hashMap_add_enableDuplicates(map,300,420);
    int firstkey = hashMap_remove(map,300);
    printf("firsstkeywassss %d\n",firstkey);
    hashMap_add_enableDuplicates(map,300,firstkey);
    int rm = 0;
    while (rm!=firstkey) {

        rm = hashMap_remove(map,300);
        printf("rm is %d\n",rm);
        hashMap_add_enableDuplicates(map,300,rm);
    }

    //hashMap_remove(map,200);
    //hashMap_add(map,200,666);
    //hashMap_remove(map,2300);
    printMap(map);
    printf("value of 300: %d\n",hashMap_get(map,300));*/


    int rc1 = pthread_create(&p,NULL,&startServer,(void*)mainServer);
    int rc2 = pthread_create(&p2,NULL,&listenForCommands,(void*)cmd);
    void * rval1;
    void * rval2;
    rc1 = pthread_join(p, &rval1);
    rc2 = pthread_join(p2,&rval2);

}
