#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int processPacket(char * data) {
    char *token;
    token=strtok(data," ");
    int t=0;
    while( token != NULL ) {
        if (t==0) { //first word in packet should be integer
            int read = strtol(token,NULL,10);
            printf("success read %d\n",read);
            return 1;
        }
        //if (t==1) {bl=strtod(token,NULL);} //set balance
        //if (t==2) {tp=strtol(token,NULL,10);} //set type
        token = strtok(NULL, " ");
        t++;
    }
    return -1; //bad packet
}
