#include "NPC.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*typedef struct {
    Entity* entity;
    int* npcID;
    int* curHealth;
    int* maxHealth;
} NPC;
*/

Npc* newNPC() {
    return (Npc*)malloc(sizeof(Npc));
}
void initNPC(Npc* n, int id, int x, int y,int z, int hp, char* name) {
    n->entity = newEntity();
    initEntity(n->entity,0,0,0);
    setEntityCoordinates(n->entity,x,y,z);
    setEntityName(n->entity,name);
    n->curHealth = malloc(sizeof(int));
    n->maxHealth = malloc(sizeof(int));
    n->serverID = malloc(sizeof(int));
    n->npcID = malloc(sizeof(int));
    n->powerLevel = malloc(sizeof(int));
    for (int i=0; i<NUM_NPC_CLICK_OPTIONS; i++) {
        n->options[i]=malloc(sizeof(int));
        *(n->options[i])=0;
    }
    *(n->powerLevel)=1;
    *(n->curHealth)=hp;
    *(n->maxHealth)=hp;
    *(n->npcID) = id;
    *(n->serverID) = -1;
}
void deleteNPC(Npc* n) {
    deleteEntity(n->entity);
    free(n->curHealth);
    free(n->maxHealth);
    free(n->npcID);
    free(n->serverID);
}

/* initNPCFromFile(n):
    @param n: the NPC to initialize
    initializes an NPC's stats using data loaded from a text file
    NPC data is stored in data/npc/npcdata.txt
*/
void initNPCFromFile(Npc* n, int id) {
    char * filePath = "data/npc/npcdata.txt";
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        FILE *fp;
        char line[256];
        //initAccount(me,fileName,0,0); //allocate memory
        if ((fp = fopen(filePath,"r"))!=NULL) {
            int i=0;
            int lineNum = 0;
            int found = 0;
            while (fgets(line, sizeof(line), fp)) { //processing line by line
                char *token;
                token=strtok(line," ");
                int t=0;
                int j=0;
                int ignore = 0;
                int datID = 0;
                int hpset=0;
                int hpsetTo = 0;
                int nameset = 0;
                char* nameSetTo = malloc(50);
                int optionSet=0;
                int optionTo[NUM_NPC_CLICK_OPTIONS];
                int levelSet=0;
                int levelSetTo=0;
                for (int i=0; i<NUM_NPC_CLICK_OPTIONS; i++) {
                    optionTo[i]=0;
                }
                if (strcmp(token,"#")==0) { //comments
                    ignore = 1;
                }
                else if (strcmp(token,"EOF")==0) {
                    return;
                } else {
                    datID = strtol(token,NULL,10);
                    if (datID==id) {
                        found=1;
                    } else {
                        found=0;
                    }
                }
                while( token != NULL ) { //processing tokens in a line
                    if (ignore) {
                        token = strtok(NULL, " ");
                        t++;
                    } else { //processing words in non-comment lines
                        if (found) {
                            if (strcmp(token,"hp")==0) {
                                hpset=1;
                            }
                            else if (strcmp(token,"n")==0) {
                                nameset=1;
                                strcpy(nameSetTo,"");

                            }
                            else if (strcmp(token,"op")==0) {
                                optionSet=1;
                            }
                            else if (strcmp(token,"lvl")==0) {
                                levelSet=1;
                            }
                            else if (hpset) {
                                hpsetTo = strtol(token,NULL,10);
                                //*(n->curHealth)=hpsetTo;
                                //*(n->maxHealth)=hpsetTo;
                                hpset=0;
                            }
                            else if (nameset>0) {
                                nameset=2;
                                strcat(nameSetTo,token);
                                /*printf("tsrlen %lu\n",strlen(token));
                                for (int i=0; i<strlen(token); i++) {
                                    *(nameSetTo+j)=*(token+i);
                                    j++;
                                }*/
                            }
                            else if (optionSet) {
                                int to = strtol(token,NULL,10);
                                optionTo[to]=1;
                                optionSet=0;
                            }
                            else if (levelSet) {
                                levelSetTo=strtol(token,NULL,10);
                                levelSet=0;
                            }
                        }
                        token = strtok(NULL, " ");
                        t++;
                        if (token!=NULL && nameset==2 && found) {
                            strcat(nameSetTo," ");
                            //*(nameSetTo+j)=' ';
                            //j++;
                        }
                        else if (token==NULL && nameset==2 && found) {
                            char* newname = malloc(50);
                            strncat(newname,nameSetTo,strlen(nameSetTo)-1);
                            initNPC(n,id,0,0,0,hpsetTo,newname);
                            for (int i=0; i<NUM_NPC_CLICK_OPTIONS; i++) {
                                if (optionTo[i])
                                    addNPCClickOption(n,i);
                            }
                            setNPCPowerLevel(n,levelSetTo);
                        }
                    }
                }
                lineNum++;
                //printf("\n");
                i++;
            }
        }
        fclose(fp);
    }
    else {
        printf("[NPC] Data file not found.\n");
    }
}

/* setNPCCoords(n,x,y,z);
    @param n: the NPC to move
    @param x,y,z: coordinates to move the NPC to
    this moves the npc's associated entity
*/
void setNPCCoords(Npc* n, int x, int y, int z) {
    setEntityCoordinates(n->entity,x,y,z);
}


char* npcName(Npc* n) {
    return n->entity->name;
}

int npcX(Npc* n) {
    return *(n->entity->absX);
}
int npcY(Npc* n) {
    return *(n->entity->absY);
}
int npcZ(Npc* n) {
    return *(n->entity->absZ);
}

void printNPC(Npc* n) {
    printf("[NPC] id=%d, level=%d, hp=%d, coords=%d,%d,%d, name=<%s> (server id = %d)\n",*(n->npcID),*(n->powerLevel),*(n->maxHealth),npcX(n),npcY(n),npcZ(n),npcName(n),*(n->serverID));
}

void addNPCClickOption(Npc* n, int op) {
    if (op>=NUM_NPC_CLICK_OPTIONS)
        return;
    printf("[NPC] adding click option %d to %s\n",op,npcName(n));
    *(n->options[op])=1;
}
void removeNPCClickOption(Npc* n, int op) {
    if (op>=NUM_NPC_CLICK_OPTIONS)
        return;
    *(n->options[op])=0;
}

void setNPCPowerLevel(Npc* n, int to) {
    printf("[NPC] set %s power level to %d\n",npcName(n),to);
    *(n->powerLevel)=to;
}
