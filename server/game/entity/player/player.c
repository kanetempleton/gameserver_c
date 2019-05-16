#include "player.h"
#include "stdlib.h"

Player * newPlayer() {
    return malloc(sizeof(Player));
}
void initPlayer(Player * p);
void deletePlayer(Player * p);

void setPlayerCoords(Player * p, int x, int y);
