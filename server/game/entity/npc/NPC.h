#ifndef H_NPC
#define H_NPC
#include "../Entity.h"

#define NUM_NPC_CLICK_OPTIONS 2

typedef struct {
    Entity* entity;
    int* npcID;
    int* serverID;
    int* curHealth;
    int* maxHealth;
    int* options[NUM_NPC_CLICK_OPTIONS];
    int* powerLevel;
} Npc;

Npc* newNPC();
void initNPC(Npc* n, int id, int x, int y, int z, int hp, char* name);
void deleteNPC(Npc* n);

/* initNPCFromFile(n):
    @param n: the NPC to initialize
    initializes an NPC's stats using data loaded from a text file
    NPC data is stored in data/npc/stats.txt
*/
void initNPCFromFile(Npc* n, int id);


/* setNPCCoords(n,x,y,z);
    @param n: the NPC to move
    @param x,y,z: coordinates to move the NPC to
    this moves the npc's associated entity
*/
void setNPCCoords(Npc* n, int x, int y, int z);


//access methods
char* npcName(Npc* n);
int npcX(Npc* n);
int npcY(Npc* n);
int npcZ(Npc* n);

void printNPC(Npc* n);

void addNPCClickOption(Npc* n, int op);
void removeNPCClickOption(Npc* n, int op);

void setNPCPowerLevel(Npc* n, int to);
#endif
