#include "player.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../../game.h"
#include "../../../net/server.h"
#include "../../../net/communication/Codes.h"

Player * newPlayer() {
    return malloc(sizeof(Player));
}
void initPlayer(Player * p, char* name, int fd, int id) {
    p->playerId = malloc(sizeof(int));
    p->playerFd = malloc(sizeof(int));
    p->absX = malloc(sizeof(int));
    p->absY = malloc(sizeof(int));
    *(p->playerId) = id;
    *(p->playerFd) = fd;
    *(p->absX) = -1;
    *(p->absY) = -1;
    p->playerName = name;

}
void deletePlayer(Player * p) {
    free(p->playerId);
    free(p->playerFd);
    free(p->absX);
    free(p->absY);
    //free(p->playerN)
}

void createNewPlayer(Player * p) { //initialize fields to what they should be for new players
    *(p->absX)=START_X;
    *(p->absY)=START_Y;
}


void sendPlayerDataToClient(Player * p) {
    printf("sending data to client\n");
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

void savePlayerInfo(Player * p) {
    printf("saving data for player %s\n",p->playerName);
    char * filePath = malloc(sizeof(char)*(strlen(p->playerName)+strlen("data/player/.txt")+1));
    printf("malloc'd file path\n");
    strcpy(filePath,"data/player/");
    strcat(filePath,p->playerName);
    strcat(filePath,".txt");
    printf("did strcat filepath is %s\n",filePath);
    FILE * fp;
    fp = fopen(filePath,"w");
    //save player info
    printf("opened file path \n");
    fprintf(fp,"%d ",*(p->absX));
    fprintf(fp,"%d \n",*(p->absY));
    //end save player info
    fprintf(fp,"EOF");
    printf("saved stuff\n");
    fclose(fp);
    free(filePath);
    printf("closed file path\n");
    sendPlayerDataToClient(p);
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
        sendPlayerDataToClient(p);
    }
    else { //new player
        printf("file not found make it\n");
        createNewPlayer(p);
        savePlayerInfo(p);
    }
    free(filePath);
}

void setPlayerCoords(Player * p, int x, int y) {
    *(p->absX) = x;
    *(p->absY) = y;
    savePlayerInfo(p);
}
