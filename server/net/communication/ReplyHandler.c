#include <stdlib.h>
#include "ReplyHandler.h"
#include <string.h>
#include "Codes.h"
#include <stdio.h>
#include "../../game/login/Database.h"
#include "SendMessage.h"
#include <math.h>
#include "../../game/map/MapData.h"

char* getReply(Server* s, int from, char* rcv) {
    char *token;
    token=strtok(rcv,SPLIT);
    int t=0;
    int logReq = 0;
    int fromId = -1;
    int updatePosReq = 0;
    char* name = malloc(sizeof(char)*MAXIMUM_USERNAME_LENGTH);
    char* pass = malloc(sizeof(char)*MAXIMUM_PASSWORD_LENGTH);
    char* mapdat = malloc(505);
    int x = -1;
    int y = -1;
    int mapedit = 0;
    int publicChatSend = -1;
    while( token != NULL ) {
        switch (t) {
            case 0: //first word in packet; always client signature
                if (strcmp(token,CLIENT_SIGNATURE)==0) {
                    //do nothing
                }
                else {
                    printf("invalid packet. kicking\n");
                    return BAD_PACKET;
                }
                break;

            case 1: //second word in packet; should be player's id
                if (strcmp(token,"-1")==0) { //login requests
                    logReq = 1;
                }
                else {
                    fromId = strtol(token,NULL,10);
                }
                break;
            case 2://action
                if (logReq) {
                    if (strcmp(token,LOGIN_REQUEST)!=0) {
                        //id of -1 not sending login request
                        return BAD_PACKET;
                    }
                }
                if (strcmp(token,PLAYER_INFO_REQUEST)==0) {
                    Player* p = getPlayer(s->game,fromId);
                    loadPlayerInfo(p);
                    //
                    sendPlayerDataToClient(s->game,p);

                }
                else if (strcmp(token,PLAYER_MOVEMENT_REQUEST)==0) {
                    updatePosReq = 1;
                }
                else if (strcmp(token,PLAYER_LOGOUT_REQUEST)==0) {
                    //logoutPlayer();
                    sendLogoutSignalToPlayer(getPlayer(s->game,fromId));
                    logoutPlayer(s->game,getPlayer(s->game,fromId));
                }
                else if (strcmp(token,PLAYER_LOGIN_MAP_REQUEST)==0) {
                    int sec = computeMapDataSection(*(getPlayer(s->game,fromId)->absX),*(getPlayer(s->game,fromId)->absY));
                    int ce = ceil(log10(sec));
                    char mapStringSend[MAP_WIDTH*MAP_HEIGHT*3+3];
                    getMapStringForChunk(sec,mapStringSend);
                    //char mapdataBuf[ce+1];
                    //sprintf(mapdataBuf,"%d",sec);
                    sendMapToPlayer(s->game,getPlayer(s->game,fromId),5,mapStringSend);
                    fetchPlayersInMapSection(s->game,getPlayer(s->game,fromId));
                    //addPlayerToMapTable(s->game,getPlayer(s->game,fromId));
                    //broadcastExistenceInMapSection(s->game,getPlayer(s->game,fromId),*(getPlayer(s->game,fromId)->absX),*(getPlayer(s->game,fromId)->absY));

                    /*getMapStringForChunk(sec+10000,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),6,mapStringSend);

                    getMapStringForChunk(sec-9999,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),7,mapStringSend);

                    getMapStringForChunk(sec+1,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),8,mapStringSend);

                    getMapStringForChunk(sec+10001,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),9,mapStringSend);

                    getMapStringForChunk(sec-10000,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),4,mapStringSend);

                    getMapStringForChunk(sec+9999,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),3,mapStringSend);

                    getMapStringForChunk(sec-1,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),2,mapStringSend);

                    getMapStringForChunk(sec-10001,mapStringSend);
                    sendMapToPlayer(getPlayer(s->game,fromId),1,mapStringSend);*/

                }
                else if (strcmp(token,SEND_MAP_EDIT)==0) {
                    mapedit=1;
                }
                else if (strcmp(token,SEND_PUBLIC_CHAT)==0) {
                    publicChatSend=1;
                }
                break;
            case 3://start of args
                if (updatePosReq)
                    x = strtol(token,NULL,10);
                else if (logReq) {
                    strcpy(name,token);
                }
                else if (mapedit) {
                    mapedit = strtol(token,NULL,10);
                }
                else if (publicChatSend==1) {
                    //publicChatSend=strtol(token,NULL,10);
                    int sec = computeMapDataSection(*(getPlayer(s->game,fromId)->absX),*(getPlayer(s->game,fromId)->absY));
                    actionToPlayersInMapSection(s->game,sec,sendPublicChatOfPlayerTo,getPlayer(s->game,fromId),token);
                }
                break;
            case 4:
                if (updatePosReq)
                    y = strtol(token,NULL,10);
                else if (logReq)
                    strcpy(pass,token);
                else if (mapedit>0) {
                    strcpy(mapdat,token);
                    saveMapdata(mapedit,mapdat);
                    printf("saved map file for %d\n",mapedit);
                }
                break;
        }
        token = strtok(NULL, SPLIT); //next token
        t++; //increase word count
    }
    //do actions after procesing the packet
    if (updatePosReq) {
        if (tileWalkable(getTileAt(x,y))) {
            Player* p = getPlayer(s->game,fromId);
            int oldX = *(p->absX);
            int oldY = *(p->absY);
            *(p->lastX) = oldX;
            *(p->lastY) = oldY;
            setPlayerCoords(p,x,y);
            sendPlayerCoordinatesToClient(s->game,p);
            int sec = computeMapDataSection(x,y);
            int oldsec = computeMapDataSection(*(p->lastX),*(p->lastY));
            if (sec!=oldsec) {
                //sendPlayerExitTo();
                removePlayerFromMapSection(s->game,p,oldsec,0);
            }
        }
        //broadcastExistenceInMapSection(s->game,getPlayer(s->game,fromId),oldX,oldY);
        //broadcastPlayerPresence(p,oldX,oldY);
        //messageToAll(SHOW_PLAYER);
    }
    else if (logReq) {
        switch (loginCheck(name,pass)) {
            case LOGIN_SUCCESS:
                printf("%s has successfully logged in.\n",name);
                Player * p = newPlayer();
                initPlayer(p,name,from,*(s->game->nextPlayerId));
                registerPlayerWithID(p,s,from);
                //addPlayer(s->game,name,from);
                //free(name);
                //free(pass);
                return "";
            case LOGIN_INVALID:
                printf("invalid password for %s\n",name);
                p = newPlayer();
                initPlayer(p,name,from,*(s->game->nextPlayerId));
                sendInvalidLoginNotification(p);
                //free(name);
                //free(pass);
                return "";
            case LOGIN_NEW:
                printf("%s does not have a profile. creating one...\n",name);
                p = newPlayer();
                initPlayer(p,name,from,*(s->game->nextPlayerId));
                setPlayerToNew(p);
                registerPlayerWithID(p,s,from);
                //addPlayer(s->game,name,from);
                //free(name);
                //free(pass);
                return "";
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
    /*printf("register player\n");
    char* buildTxt = malloc(sizeof(char)*25);
    strcpy(buildTxt,REGISTER_PLAYER_WITH_ID);
    strcat(buildTxt,SPLIT);
    char idbuf[3];
    sprintf(idbuf,"%d",id);
    strcat(buildTxt,idbuf);
    messageToClient(fd,buildTxt);
    free(buildTxt);*/
}




/*


t=0

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



t=1

if (logReq) {
    strcpy(name,token);
    printf("username is:%s\n",token);
}
else if (updatePosReq) {
    x = strtol(token,NULL,10);
}



t=2
if (logReq) {
    strcpy(pass,token);
    printf("password is:%s\n",token);
}
else if (updatePosReq) {
    y = strtol(token,NULL,10);
    setPlayerCoords(getPlayer_fd(s->game,from),x,y);

}

*/
