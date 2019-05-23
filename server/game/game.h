#ifndef H_GAME
#define H_GAME
#define MAX_PLAYERS 30
#define START_X 250
#define START_Y 250
#include "entity/player/player.h"
typedef struct {
    int * numPlayers;
    int * nextPlayerId;
    Player * players[MAX_PLAYERS];
} Game;
Game* newGame();
void initGame(Game* g);
void deleteGame(Game* g);
void addPlayer(Game* g,char* name, int fd);
void removePlayer(Game* g,int id);
void * runGame(void * arg);
Player * getPlayer_fd(Game* g,int fd); //probably should never use this
Player * getPlayer_id(Game* g, int id);
Player * getPlayer_name(Game*g, char* n);
#endif
