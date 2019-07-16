#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int * playerId;
    int * playerFd;
    char * playerName;
    int * absX;
    int * absY;
    int * lastX;
    int * lastY;
    int * inMap;
    int * loggedIn;
} Player;
Player * newPlayer();
void initPlayer(Player * p, char* name, int fd, int id);
void deletePlayer(Player * p);
void setPlayerCoords(Player * p, int x, int y);
void savePlayerInfo(Player * p);
void loadPlayerInfo(Player * p);
void setPlayerToNew(Player * p);
#endif
