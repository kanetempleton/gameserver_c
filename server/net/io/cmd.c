#include "cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../server.h"
#include "../../util/HashMap.h"

CMD * newCMD() {
    return (CMD*)malloc(sizeof(CMD));
}
void initCMD(CMD * me, Server * s) {
    me->linked = malloc(sizeof(Server));
    me->linked = s;
}
void deleteCMD(CMD * me) {
    free(me->linked);
    free(me);
}

void processCommand(CMD * me, char buf[]) {
    char *token;
    token=strtok(buf," ");
    int t=0;
    int msgToC = 0;
    while( token != NULL ) {
        //while (strcmp(check,input) != 0)
        if (t==0) {
            if (strcmp("talk",token) == 0) {

            }
            else if (strcmp("mapplayers",token)==0) {
                printMap(me->linked->game->playersByMapSection);
            }
            else if (strcmp("players",token)==0) {
                
            }
            else {
                printf("invalid command\n");
            }

        }
        if (t>=1) {

        }
        token = strtok(NULL, " ");
        t++;
    }
    if (msgToC)
        messageToClient(4,buf);
    msgToC=0;
}

void * listenForCommands(void * arg) {
    CMD * me = (CMD*)arg;
    char buf[256];
    while (1) {
        fgets(buf,256,stdin);
        strtok(buf,"\n");
        printf("input: '%s'\n",buf);
        processCommand(me,buf);
        //messageToClient(me->linked,4,buf);
    }
    return NULL;
}
