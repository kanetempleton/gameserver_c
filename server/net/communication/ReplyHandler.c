#include <stdlib.h>
#include "ReplyHandler.h"
#include <string.h>
#include "Codes.h"
#include <stdio.h>
#include "../../game/login/Database.h"

char* getReply(char* rcv) {
    char *token;
    token=strtok(rcv,SPLIT);
    int t=0;
    int logReq = 0;
    char* name = malloc(sizeof(char)*12);
    char* pass = malloc(sizeof(char)*20);
    while( token != NULL ) {
        if (t==0) { //first word in packet
            if (strcmp(token,LOGIN_REQUEST)==0) {
                printf("trying to login\n");
                logReq=1;
            }
            else {
                printf("unsupported command in packet\n");
            }
        }
        if (t==1) {
            if (logReq) {
                strcpy(name,token);
                printf("username is:%s\n",token);
            }
        }
        if (t==2) {
            if (logReq) {
                strcpy(pass,token);
                printf("password is:%s\n",token);
            }
        }
        token = strtok(NULL, SPLIT);
        t++;
    }
    if (logReq) {
        switch (loginCheck(name,pass)) {
            case LOGIN_SUCCESS:
                printf("%s has successfully logged in.\n",name);
                return "successlogin";
            case LOGIN_INVALID:
                printf("invalid password for %s\n",name);
                return "invalidlogin";
            case LOGIN_NEW:
                printf("%s does not have a profile. creating one...\n",name);
                return "createprofile";
            default:
                printf("Invalid login return code.\n");
                break;
        }
    }
    free(name);
    free(pass);
    return "";
}
