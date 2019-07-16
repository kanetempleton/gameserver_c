#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../../game.h"
#include "../../../net/server.h"
#include "../../../net/communication/Codes.h"
#include "../../../net/communication/SendMessage.h"

Player * newPlayer() {
    return (Player*)malloc(sizeof(Player));
}
void initPlayer(Player * p, char* name, int fd, int id) {
    p->playerId = malloc(sizeof(int));
    p->playerFd = malloc(sizeof(int));
    p->absX = malloc(sizeof(int));
    p->absY = malloc(sizeof(int));
    p->lastX = malloc(sizeof(int));
    p->lastY = malloc(sizeof(int));
    p->playerName = malloc(sizeof(char)*12);
    p->inMap = malloc(sizeof(int));
    p->loggedIn = malloc(sizeof(int));
    *(p->playerId) = id;
    *(p->playerFd) = fd;
    *(p->absX) = -1;
    *(p->absY) = -1;
    *(p->lastX) = -1;
    *(p->lastY) = -1;
    *(p->loggedIn) = 1;
    *(p->inMap) = -1;
    strcpy(p->playerName,name);
    /*for (int i=0; i<strlen(name); i++) {
        *(p->playerName+i)=name[i];
    }*/
}
void deletePlayer(Player * p) {
    free(p->playerId);
    free(p->playerFd);
    free(p->absX);
    free(p->absY);
    free(p->lastX);
    free(p->lastY);
    free(p->inMap);
    free(p->playerName);
}

void setPlayerToNew(Player * p) { //initialize fields to what they should be for new players
    *(p->absX)=START_X;
    *(p->absY)=START_Y;
}


void savePlayerInfo(Player * p) {
    //printf("saving data for player %s\n",p->playerName);
    char * filePath = malloc(sizeof(char)*(strlen(p->playerName)+strlen("data/player/.txt")+1));
    strcpy(filePath,"data/player/");
    strcat(filePath,p->playerName);
    strcat(filePath,".txt");
    FILE * fp;
    fp = fopen(filePath,"w");
    //save player info
    fprintf(fp,"%d ",*(p->absX));
    fprintf(fp,"%d \n",*(p->absY));
    //end save player info
    fprintf(fp,"EOF");
    fclose(fp);
    free(filePath);
    //sendPlayerDataToClient(p);
}

void loadPlayerInfo(Player * p) { //called on login
    char * filePath = malloc(sizeof(char)*strlen(p->playerName)+sizeof(char)*strlen("data/player/.txt"));
    strcpy(filePath,"data/player/");
    strcat(filePath,p->playerName);
    strcat(filePath,".txt");
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        printf("retrieving data for %s\n",p->playerName);
        FILE *fp;
        char line[256];
        //initAccount(me,fileName,0,0); //allocate memory
        if ((fp = fopen(filePath,"r"))!=NULL) {
            int i=0;
            int lineNum = 0;
            while (fgets(line, sizeof(line), fp)) {
                char *token;
                //char *nm;
                //double bl;
                //int tp;
                token=strtok(line," ");
                int t=0;
                //printf("line:");
                int x=-1;
                int y=-1;
                while( token != NULL ) {
                    if (t==0 && lineNum==0) { // load x coordinate
                        x=strtol(token,NULL,10);
                    }
                    if (t==1 && lineNum==0) { //load y coordinate
                        y=strtol(token,NULL,10);
                        *(p->absX)=x;
                        *(p->absY)=y;
                    }
                    //if (t==1) {bl=strtod(token,NULL);} //set balance
                    //if (t==2) {tp=strtol(token,NULL,10);} //set type
                    //printf("%s",token);
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
    else { //new player
        printf("file not found make it\n");
        setPlayerToNew(p);
        savePlayerInfo(p);
    }
    free(filePath);
}

void setPlayerCoords(Player * p, int x, int y) {
    *(p->absX) = x;
    *(p->absY) = y;
    savePlayerInfo(p);
}
