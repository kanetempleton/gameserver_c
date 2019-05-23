#include <stdlib.h>
#include "ReplyHandler.h"
#include <string.h>
#include "Codes.h"
#include <stdio.h>
#include "../../game/login/Database.h"

char* getReply(Server* s, int from, char* rcv) {
    char *token;
    token=strtok(rcv,SPLIT);
    int t=0;
    int logReq = 0;
    int updatePosReq = 0;
    char* name = malloc(sizeof(char)*12);
    char* pass = malloc(sizeof(char)*20);
    int x = -1;
    int y = -1;
    while( token != NULL ) {
        if (t==0) { //first word in packet
            if (strcmp(token,LOGIN_REQUEST)==0) {
                printf("trying to login\n");
                logReq=1;
            }
            else if (strcmp(token,REQUEST_FOR_PLAYER_DATA)==0) {
                printf("got info request. sending it...\n");
                sendPlayerDataToClient(getPlayer_fd(s->game,from));
                return "";
            }
            else if (strcmp(token,PLAYER_MOVEMENT_REQUEST)==0) {
                printf("got a movement request\n");
                updatePosReq=1;
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
            else if (updatePosReq) {
                x = strtol(token,NULL,10);
            }
        }
        if (t==2) {
            if (logReq) {
                strcpy(pass,token);
                printf("password is:%s\n",token);
            }
            else if (updatePosReq) {
                y = strtol(token,NULL,10);
                setPlayerCoords(getPlayer_fd(s->game,from),x,y);
            }
        }
        token = strtok(NULL, SPLIT);
        t++;
    }
    if (logReq) {
        switch (loginCheck(name,pass)) {
            case LOGIN_SUCCESS:
                printf("%s has successfully logged in.\n",name);
                addPlayer(s->game,name,from);
                free(name);
                free(pass);
                return SUCCESS_LOGIN;
            case LOGIN_INVALID:
                printf("invalid password for %s\n",name);
                free(name);
                free(pass);
                return INVALID_LOGIN;
            case LOGIN_NEW:
                printf("%s does not have a profile. creating one...\n",name);
                addPlayer(s->game,name,from);
                free(name);
                free(pass);
                return NEWPROFILE_LOGIN;
            default:
                printf("Invalid login return code.\n");
                break;
        }
    }
    free(name);
    free(pass);
    return "";
}

void registerPlayer(int fd, int id, Player* p) {
    printf("register player\n");
    char* buildTxt = malloc(sizeof(char)*25);
    strcpy(buildTxt,REGISTER_PLAYER_WITH_ID);
    strcat(buildTxt,SPLIT);
    char idbuf[3];
    sprintf(idbuf,"%d",id);
    strcat(buildTxt,idbuf);
    messageToClient(fd,buildTxt);
    free(buildTxt);
}
