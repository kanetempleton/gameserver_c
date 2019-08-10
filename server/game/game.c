#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include "../net/communication/ReplyHandler.h"
#include "../net/communication/SendMessage.h"
#include "../util/MutableList.h"
#include "map/MapData.h"
#include <unistd.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t gameLock;

Game* newGame() {
    return (Game*)malloc(sizeof(Game));
}
void initGame(Game* g) {
    getTileAt(241,246);
    getTileAt(241,245);
    pthread_mutex_init(&gameLock,NULL);
    g->numPlayers = malloc(sizeof(int));
    g->nextPlayerId = malloc(sizeof(int));
    *(g->numPlayers) = 0;
    *(g->nextPlayerId) = 0;
    g->playersByMapSection = newHashMap();
    initHashMap(g->playersByMapSection);

    g->numNPCs = malloc(sizeof(int));
    g->nextNPCId = malloc(sizeof(int));
    *(g->numNPCs)=0;
    *(g->nextNPCId)=0;
    autospawnNPCs(g);
    //actionToPlayersInMapSection(g, printNumber);
}
void deleteGame(Game* g) {
    free(g->numPlayers);
}
void addPlayer(Game* g, char* name, int fd) { //todo make this reject if max players reached
    Player * p = newPlayer();
    initPlayer(p,name,fd,*(g->nextPlayerId));
    printf("adding player %s to game with id %d for sock %d\n",name,*(g->nextPlayerId),fd);
    g->players[*(g->nextPlayerId)]=p;
    *(g->numPlayers)=*(g->numPlayers)+1;
    printf("there are now %d players online\n",*(g->numPlayers));
    registerPlayer(fd,*(g->nextPlayerId),p);
    *(g->nextPlayerId)=*(g->nextPlayerId)+1;
    //loadPlayerInfo(p);
}
void addPlayerToGame(Game* g, Player* p) {
    g->players[*(p->playerId)]=p;
    *(g->players[*(p->playerId)]->loggedIn) = 1;
    *(g->numPlayers)=*(g->numPlayers)+1;
    printf("added player %s to the game. there are now %d players online.\n",p->playerName,*(g->numPlayers));
    //addPlayerToMapTable(g,p);
    //fetchPlayersInMapSection(g,p);
}
void removePlayer(Game* g, int plrid) {
    pthread_mutex_lock(&gameLock);
    char* nom = g->players[plrid]->playerName;
    *(g->players[plrid]->loggedIn)=0;
    removePlayerFromMapTable(g,g->players[plrid]);
    deletePlayer(g->players[plrid]);
    g->players[plrid]=NULL;
    *(g->numPlayers)=*(g->numPlayers)-1;
    printf("removing player %s with id %d from game\n",nom,plrid);
    printf("there are now %d players online\n",*(g->numPlayers));
    pthread_mutex_unlock(&gameLock);
}

Player * getPlayer_fd(Game* g,int fd) {
    for (int i=0;i<*(g->nextPlayerId); i++) {
        Player * p = g->players[i];
        if (p!=NULL) {
            if (*(p->playerFd)==fd)
                return p;
        }
    }
    return NULL;
}
Player * getPlayer_id(Game* g, int id) {
    for (int i=0; i<*(g->nextPlayerId); i++) {
        Player * p = g->players[i];
        if (p!=NULL) {
            if (*(p->playerId)==id)
                return p;
        }
    }
    return NULL;
}

Player* getPlayer(Game* g, int plrid) {
    return g->players[plrid];
}

//get the next player ID and update it
int fetchID(Game* g) {
    int rv = *(g->nextPlayerId);
    *(g->nextPlayerId)=*(g->nextPlayerId)+1;
    return rv;
}
void logoutPlayer(Game* g, Player* p) {
    //char* nom = g->players[plrid]->playerName;
    pthread_mutex_lock(&gameLock);
    printf("logging out %s\n",p->playerName);
    removePlayerFromMapTable(g,p);
    int id = *(p->playerId);
    deletePlayer(p);
    g->players[id]=NULL;
    *(g->numPlayers)=*(g->numPlayers)-1;
    printf("there are now %d players online\n",*(g->numPlayers));
    pthread_mutex_unlock(&gameLock);
}

int computeMapDataSection(int x, int y) {
    int mapSectionX = x/MAP_WIDTH;
    int mapSectionY = y/MAP_HEIGHT;
    int offsetSectionX = mapSectionX + 714; //(MAX_COORDINATE/MAP_WIDTH);
    int offsetSectionY = mapSectionY + 833; //(MAX_COORDINATE/MAP_HEIGHT);
    int sumPartX =  offsetSectionX * 10000;
    int sumPartY = offsetSectionY + 100000000;
    return sumPartX+sumPartY;
}

void addPlayerToMapTable(Game* g, Player* p) {
    //hashMap_add_enableDuplicates(g->playersByMapSection,computeMapDataSection(*(p->absX),*(p->absY)),*(p->playerId));
    //printMap(g->playersByMapSection);
    int curSec = computeMapDataSection(*(p->absX),*(p->absY));
    if (*(p->inMap)!=curSec) {
        if (*(p->inMap)>=0) {
            hashMap_removeSpecific(g->playersByMapSection,*(p->inMap),*(p->playerId));
        }
        hashMap_add_enableDuplicates(g->playersByMapSection,curSec,*(p->playerId));
        *(p->inMap)=curSec;
    }
}

void removePlayerFromMapTable(Game* g, Player* p) {
    printf("removing %s from map table...\n",p->playerName);
    removePlayerFromMapSection(g,p,computeMapDataSection(*(p->absX),*(p->absY)),1);
    hashMap_removeSpecific(g->playersByMapSection,computeMapDataSection(*(p->absX),*(p->absY)),*(p->playerId));
    *(p->inMap)=-1;
}

void broadcastExistenceInMapSection(Game* g, Player* p, int fromX, int fromY) {
    printf("trying to broadcast\n");
    int sec =  computeMapDataSection(*(p->absX),*(p->absY));

    MutableList* playersInSection = newMutableList();
    initMutableList(playersInSection);
    int plid = hashMap_remove(g->playersByMapSection,sec);
    mutList_addValue(playersInSection,plid);
    while (plid!=-1) {
        printf("plidto:%d\n",plid);
        //mutList_addValue(playersInSection,plid);
        sendPlayerPresenceTo(p,getPlayer(g,plid),fromX,fromY);
        plid = hashMap_remove(g->playersByMapSection,sec);
        mutList_addValue(playersInSection,plid);
    }
    while (!mutList_atEnd(playersInSection))
        hashMap_add_enableDuplicates(g->playersByMapSection,sec,mutList_next(playersInSection));

    deleteMutableList(playersInSection);
    free(playersInSection);
}

void fetchPlayersInMapSection(Game* g, Player* p) {
    int sec = computeMapDataSection(*(p->absX),*(p->absY));
    MutableList* playersInSection = newMutableList();
    initMutableList(playersInSection);
    int plid = hashMap_remove(g->playersByMapSection,sec);
    while (plid!=-1) {
        printf("player in map section: %s\n",getPlayer(g,plid)->playerName);
        mutList_addValue(playersInSection,plid);
        if (plid!=*(p->playerId)) {
            sendPlayerPresenceTo(getPlayer(g,plid),p,*(getPlayer(g,plid)->lastX),*(getPlayer(g,plid)->lastY));
            sendPlayerPresenceTo(p,getPlayer(g,plid),*(p->lastX),*(p->lastY));
        }
        plid = hashMap_remove(g->playersByMapSection,sec);
    }
    while (!mutList_atEnd(playersInSection)) {
        hashMap_add_enableDuplicates(g->playersByMapSection,sec,mutList_next(playersInSection));
    }

    deleteMutableList(playersInSection);
    free(playersInSection);
}

void actionToPlayersInMapSection(Game* g, int sec, void (*action)(), Player* plrArg, void* arg) {
    //(*v)(5);
    MutableList* playersInSection = newMutableList();
    initMutableList(playersInSection);
    int plid = hashMap_remove(g->playersByMapSection,sec);
    while (plid!=-1) {
        //printf("player in map section: %s\n",getPlayer(g,plid)->playerName);
        mutList_addValue(playersInSection,plid);
        if (plrArg!=NULL)
            action(plrArg,getPlayer(g,plid),arg);
        else
            action(getPlayer(g,plid),arg);
        //sendPlayerPresenceTo(getPlayer(g,plid),p,*(getPlayer(g,plid)->lastX),*(getPlayer(g,plid)->lastY));
        //sendPlayerPresenceTo(p,getPlayer(g,plid),*(p->lastX),*(p->lastY));
        plid = hashMap_remove(g->playersByMapSection,sec); //
    }
    while (!mutList_atEnd(playersInSection)) {
        hashMap_add_enableDuplicates(g->playersByMapSection,sec,mutList_next(playersInSection));
    }

    deleteMutableList(playersInSection);
    free(playersInSection);
}

void printNumber(int x) {
    printf("number: %d\n",x);
}

void removePlayerFromMapSection(Game* g, Player* p, int oldSec, int sendCurPos) {
    printf("trying to remove %s from section %d\n",p->playerName,oldSec);
    MutableList* playersInSection = newMutableList();
    initMutableList(playersInSection);
    int plid = hashMap_remove(g->playersByMapSection,oldSec);
    while (plid!=-1) {
        //printf("player in map section: %s\n",getPlayer(g,plid)->playerName);
        mutList_addValue(playersInSection,plid);
        if (plid!=*(p->playerId)) {
            //sendPlayerPresenceTo(getPlayer(g,plid),p,*(getPlayer(g,plid)->lastX),*(getPlayer(g,plid)->lastY));
            //sendPlayerPresenceTo(p,getPlayer(g,plid),*(p->lastX),*(p->lastY));
            if (sendCurPos)
                sendPlayerExitTo(getPlayer(g,plid),*(p->playerId),*(p->absX),*(p->absY));
            else
                sendPlayerExitTo(getPlayer(g,plid),*(p->playerId),*(p->lastX),*(p->lastY));
        }
        plid = hashMap_remove(g->playersByMapSection,oldSec);
    }
    while (!mutList_atEnd(playersInSection)) {
        hashMap_add_enableDuplicates(g->playersByMapSection,oldSec,mutList_next(playersInSection));
    }

    deleteMutableList(playersInSection);
    free(playersInSection);
}

void saveMapdata(int section, char* data) {
    //printf("saving data for player %s\n",p->playerName);
    char * filePath = malloc(sizeof(char)*(10+strlen("data/map/.txt")+1));
    strcpy(filePath,"data/map/");
    char stupidbuffer[10];
    sprintf(stupidbuffer,"%d",section);
    strcat(filePath,stupidbuffer);
    strcat(filePath,".txt");
    FILE * fp;
    fp = fopen(filePath,"w");
    //save player info
    for (int i=0; i<strlen(data); i++) {
    //    fprintf(fp,"%d",*(data+i));
        if (i!=0 && i%42==0)
            fprintf(fp,"\n%c",*(data+i));
        else if (i%3==0 && i!=0)
            fprintf(fp," %c",*(data+i));
        else
            fprintf(fp,"%c",*(data+i));
    }
    //end save player info
    fclose(fp);
    free(filePath);
    printf("done writing map\n");
    //sendPlayerDataToClient(p);
}

void autospawnNPCs(Game* g) {
    char * filePath = "data/npc/spawn.txt";
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        FILE *fp;
        char line[256];
        if ((fp = fopen(filePath,"r"))!=NULL) {
            int i=0;
            int lineNum = 0;
            while (fgets(line, sizeof(line), fp)) { //process lines
                char *token;
                token=strtok(line," ");
                int t=0;
                int x=-1;
                int y=-1;
                int z=-1;
                int spawnID=-1;
                while( token != NULL ) { //process tokens within line
                    if (t==0) {
                        spawnID = strtol(token,NULL,10);
                    }
                    else if (t==1) {
                        x = strtol(token,NULL,10);
                    }
                    else if (t==2) {
                        y = strtol(token,NULL,10);
                    }
                    else if (t==3) {
                        z = strtol(token,NULL,10);
                        spawnNPC(g,spawnID,x,y,z);
                    }
                    token = strtok(NULL, " ");
                    t++;
                }
                lineNum++;
                //printf("\n");
                i++;
            }
        }
        fclose(fp);
    }
    else {
        printf("[NPC] Autospawn file not found.\n");
    }
}

void spawnNPC(Game* g, int id, int x, int y, int z) {
    printf("calling spawnNPC...\n");
    int sid = *(g->nextNPCId);
    if (sid>=MAX_NPCS) {
        printf("[NPC] Max number of NPCs spawned.");
        return;
    }
    g->npcs[sid] = newNPC();
    initNPCFromFile(g->npcs[sid],id);
    setNPCCoords(g->npcs[sid],x,y,z);
    *(g->nextNPCId) = *(g->nextNPCId)+1;
    *(g->npcs[sid]->serverID)=sid;
}

void * runGame(void * arg) {
    Game* g = (Game*)arg;
    while (1) { //game loop
        pthread_mutex_lock(&gameLock);
        for (int i=0; i<*(g->nextPlayerId); i++) { //process players.. good or bad?
            if (g->players[i]!=NULL) {
                if (*(g->players[i]->inMap)!=computeMapDataSection(*(g->players[i]->absX),*(g->players[i]->absY))) {
                    if (*(g->players[i]->loggedIn))
                        addPlayerToMapTable(g,g->players[i]);
                    //fetchPlayersInMapSection(g,g->players[i]);
                }
            }
        }
        /*for (int i=0; i<*(g->nextNPCId); i++) { //process NPCs
             if (g->npcs[i]!=NULL) {
                 int sec = computeMapDataSection(npcX(g->npcs[i]),npcY(g->npcs[i]));
                 actionToPlayersInMapSection(g,sec,alertPlayerOfNpc,NULL,g->npcs[i]);
             }
        }*/
        pthread_mutex_unlock(&gameLock);
    }
}
