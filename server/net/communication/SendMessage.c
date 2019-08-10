#include "SendMessage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Codes.h"
#include "../../game/game.h"

void registerPlayerWithID(Player * p, Server* s, int fd) {

    *(p->playerId) = fetchID(mainGame);
    *(p->playerFd) = fd;
    addPlayerToGame(mainGame,p);
    char* sendText = malloc(strlen(SEND_PLAYER_ID)+strlen(SPLIT)+6);
    strcpy(sendText,SEND_PLAYER_ID);
    strcat(sendText,SPLIT);
    char idbuf[5];
    sprintf(idbuf,"%d",*(p->playerId));
    strcat(sendText,idbuf);
    messageToClient(fd,sendText);
    free(sendText);

    printf("registered %s with id %d\n",p->playerName,*(p->playerId));

}


void sendPlayerCoordinatesToClient(Game* g, Player * p) {
    fetchPlayersInMapSection(g,p);
    char* sendinfotxt = malloc(sizeof(char)*(10+strlen(SEND_PLAYER_COORDS)+strlen(SPLIT)*2));
    char xbuf[4];
    char ybuf[4];
    sprintf(xbuf,"%d",*(p->absX));
    sprintf(ybuf,"%d",*(p->absY));
    strcpy(sendinfotxt,SEND_PLAYER_COORDS);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    messageToClient(*(p->playerFd),sendinfotxt);
    free(sendinfotxt);
}

void sendPlayerDataToClient(Game* g, Player* p) {
    fetchPlayersInMapSection(g,p);
    char* sendinfotxt = malloc(sizeof(char)*(10+strlen(SEND_PLAYER_DATA)+strlen(SPLIT)*2));
    char xbuf[4];
    char ybuf[4];
    sprintf(xbuf,"%d",*(p->absX));
    sprintf(ybuf,"%d",*(p->absY));
    strcpy(sendinfotxt,SEND_PLAYER_DATA);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    messageToClient(*(p->playerFd),sendinfotxt);
    free(sendinfotxt);
}

void sendInvalidLoginNotification(Player* p) {
    messageToClient(*(p->playerFd),INVALID_LOGIN);
}

void sendNewPlayerData(Player* p) {
    //fill in later
}

void sendLogoutSignalToPlayer(Player* p) {
    messageToClient(*(p->playerFd),LOGOUT_PLAYER);
}

void broadcastPlayerPresence(Player* p, int oldx, int oldy) {
    char* sendinfotxt = malloc(sizeof(char)*(20+strlen(SHOW_PLAYER)+strlen(SPLIT)*4));
    char xbuf[4];
    char ybuf[4];
    sprintf(xbuf,"%d",*(p->absX));
    sprintf(ybuf,"%d",*(p->absY));
    strcpy(sendinfotxt,SHOW_PLAYER);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    sprintf(xbuf,"%d",oldx);
    sprintf(ybuf,"%d",oldy);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    messageToAll(sendinfotxt,*(p->playerFd));
    free(sendinfotxt);
}

void sendPlayerPresenceTo(Player* pOf, Player* pTo, int oldx, int oldy) {
    char* sendinfotxt = malloc(sizeof(char)*(25+strlen(SHOW_PLAYER)+strlen(SPLIT)*5));
    char xbuf[4];
    char ybuf[4];
    sprintf(xbuf,"%d",*(pOf->playerId));
    sprintf(ybuf,"%d",*(pOf->absY));
    strcpy(sendinfotxt,SHOW_PLAYER);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    sprintf(xbuf,"%d",*(pOf->absX));
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    sprintf(xbuf,"%d",oldx);
    sprintf(ybuf,"%d",oldy);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    //messageToAll(sendinfotxt,*(p->playerFd));
    printf("TRYING TO SEND THE PLAYER' PRESENCE %d %d\ns",*(pOf->playerId),*(pTo->playerFd));
    messageToClient(*(pTo->playerFd),sendinfotxt);
    free(sendinfotxt);
}

void sendMapToPlayer(Game* g, Player* p, int mapNum, char* data) {
    //fetchPlayersInMapSection(g,p);
    char* sendinfotxt = malloc(sizeof(char)*(strlen(data)+3+strlen(SEND_PLAYER_MAP)+strlen(SPLIT)*2));
    char mapNumBuf[2];
    //char ybuf[4];
    sprintf(mapNumBuf,"%d",mapNum);
    //sprintf(ybuf,"%d",*(p->absY));
    strcpy(sendinfotxt,SEND_PLAYER_MAP);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,mapNumBuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,data);
    messageToClient(*(p->playerFd),sendinfotxt);
    free(sendinfotxt);

    //sendInfoMessage(p,"testing testing swag swag swag 69 420 666");
}

void sendPlayerExitTo(Player* pTo, int idOf, int fromX, int fromY) {
    char* sendinfotxt = malloc(sizeof(char)*(20+strlen(HIDE_PLAYER)+strlen(SPLIT)*3));
    char xbuf[4];
    char ybuf[4];
    char idbuf[4];
    sprintf(xbuf,"%d",fromX);
    sprintf(ybuf,"%d",fromY);
    sprintf(idbuf,"%d",idOf);
    strcpy(sendinfotxt,HIDE_PLAYER);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,idbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,xbuf);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,ybuf);
    //messageToAll(sendinfotxt,*(p->playerFd));
    printf("TRYING TO SEND THE PLAYER' EXIT %d %d\n",idOf,*(pTo->playerId));
    messageToClient(*(pTo->playerFd),sendinfotxt);
    free(sendinfotxt);
}

void sendInfoMessage(Player* p, char* msg) {
    char* sendinfotxt = malloc(sizeof(char)*(1+strlen(msg)+strlen(SEND_INFOBOX_MSG)+strlen(SPLIT)));
    strcpy(sendinfotxt,SEND_INFOBOX_MSG);
    strcat(sendinfotxt,SPLIT);
    strcat(sendinfotxt,msg);
    //messageToAll(sendinfotxt,*(p->playerFd));
    //printf("TRYING TO SEND THE PLAYER' EXIT %d %d\n",idOf,*(pTo->playerId));
    messageToClient(*(p->playerFd),sendinfotxt);
    free(sendinfotxt);
}

void sendPublicChatOfPlayerTo(Player* pOf, Player* pTo, char* msg) {
    char* sendinfo = malloc(1+strlen(pOf->playerName)+strlen(msg)+strlen(PUBLIC_CHAT_FROM)+strlen(SPLIT)*2);
    strcpy(sendinfo,PUBLIC_CHAT_FROM);
    strcat(sendinfo,SPLIT);
    strcat(sendinfo,pOf->playerName);
    strcat(sendinfo,SPLIT);
    strcat(sendinfo,msg);
    messageToClient(*(pTo->playerFd),sendinfo);
    free(sendinfo);
}

void alertPlayerOfNpc(Player * p, Npc* n) {
    char* sendinfo = malloc(26+strlen(SHOW_NPC)+strlen(SPLIT)*6+strlen(npcName(n)));
    strcpy(sendinfo,SHOW_NPC);
    strcat(sendinfo,SPLIT);
    char* idbuf = malloc(5);
    sprintf(idbuf,"%d",*(n->npcID));
    strcat(sendinfo,idbuf);
    strcat(sendinfo,SPLIT);
    char* xbuf = malloc(5);
    char* ybuf = malloc(5);
    sprintf(xbuf,"%d",npcX(n));
    sprintf(ybuf,"%d",npcY(n));
    strcat(sendinfo,xbuf);
    strcat(sendinfo,SPLIT);
    strcat(sendinfo,ybuf);
    strcat(sendinfo,SPLIT);
    char* lvlbuf = malloc(10);
    sprintf(lvlbuf,"%d",*(n->powerLevel));
    strcat(sendinfo,lvlbuf);
    strcat(sendinfo,SPLIT);
    for (int i=0; i<NUM_NPC_CLICK_OPTIONS; i++) {
        char* optionbuf = malloc(2);
        sprintf(optionbuf,"%d",*(n->options[i]));
        strcat(sendinfo,optionbuf);
        free(optionbuf);
    }
    strcat(sendinfo,SPLIT);
    strcat(sendinfo,npcName(n));
    messageToClient(*(p->playerFd),sendinfo);
    free(sendinfo);
    free(xbuf);
    free(ybuf);
    free(idbuf);
    free(lvlbuf);
}
