#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include "../net/communication/ReplyHandler.h"

Game* newGame() {
    return (Game*)malloc(sizeof(Game));
}
void initGame(Game* g) {
    g->numPlayers = malloc(sizeof(int));
    g->nextPlayerId = malloc(sizeof(int));
    *(g->numPlayers) = 0;
    *(g->nextPlayerId) = 0;
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
void removePlayer(Game* g, int plrid) {
    char* nom = g->players[plrid]->playerName;
    deletePlayer(g->players[plrid]);
    g->players[plrid]=NULL;
    *(g->numPlayers)=*(g->numPlayers)-1;
    printf("removing player %s with id %d from game\n",nom,plrid);
    printf("there are now %d players online\n",*(g->numPlayers));
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
void * runGame(void * arg) {
    Game* me = (Game*)arg;
    while (1) { //game loop
        //do nothing
        //printf("game loop\n");
    }
}
