#include "MapData.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../game.h"

void getMapStringForChunk(int mapid, char* copyTo) {
    char * filePath = malloc(sizeof(char)*10+sizeof(char)*strlen("data/map/.txt"));
    strcpy(filePath,"data/map/");
    char mapIDBuf[10];
    sprintf(mapIDBuf,"%d",mapid);
    strcat(filePath,mapIDBuf);
    strcat(filePath,".txt");
    char* fillWithMapData = malloc(MAP_WIDTH*MAP_HEIGHT*3);
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        FILE *fp;
        char line[256];
        //initAccount(me,fileName,0,0); //allocate memory
        if ((fp = fopen(filePath,"r"))!=NULL) {
            int i=0;
            int j=0;
            int lineNum = 0;
            while (fgets(line, sizeof(line), fp)) {
                char *token;
                //char *nm;
                //double bl;
                //int tp;
                token=strtok(line," ");
                int t=0;
                //printf("line:");
                while( token != NULL ) {
                    //printf("TOKEN: %s\n",token);
                    for (int k=0; k<3; k++) {
                        *(fillWithMapData+(j++))=token[k];
                    }
                    //fillWithMapData[j]=line[j];
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
    } else { //File does not exist - send blank map data
        printf("mapdata not found for %d\n",mapid);
        for (int i=0; i<MAP_WIDTH*MAP_HEIGHT*3; i++) {
            *(fillWithMapData+i)='0';
        }
    }

    //printf("mapdata: <%s>\n",fillWithMapData);
    free(filePath);
    /*if (strcmp(user,"admin")==0) {
        if (strcmp(pass,"test")==0)
            return LOGIN_SUCCESS;
        else
            return LOGIN_INVALID;
    }*/
    strcpy(copyTo,fillWithMapData);
    free(fillWithMapData);
}

int getTileAt(int x, int y) {
    int mapid = computeMapDataSection(x,y);
    char * filePath = malloc(sizeof(char)*10+sizeof(char)*strlen("data/map/.txt"));
    strcpy(filePath,"data/map/");
    char mapIDBuf[10];
    sprintf(mapIDBuf,"%d",mapid);
    strcat(filePath,mapIDBuf);
    strcat(filePath,".txt");
    char* fillWithMapData = malloc(MAP_WIDTH*MAP_HEIGHT*3);
    char* returnTile = malloc(4);
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        FILE *fp;
        char line[256];
        //initAccount(me,fileName,0,0); //allocate memory
        if ((fp = fopen(filePath,"r"))!=NULL) {
            int i=0;
            int j=0;
            int lineNum = 0;
            while (fgets(line, sizeof(line), fp)) {
                char *token;
                //char *nm;
                //double bl;
                //int tp;
                token=strtok(line," ");
                int t=0;
                //printf("line:");
                while( token != NULL ) {
                    //printf("TOKEN: %s\n",token);
                    for (int k=0; k<3; k++) {
                        *(fillWithMapData+(j++))=token[k];
                    }
                    //fillWithMapData[j]=line[j];
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
    } else { //File does not exist - send blank map data
        printf("mapdata not found for %d\n",mapid);
        for (int i=0; i<MAP_WIDTH*MAP_HEIGHT*3; i++) {
            *(fillWithMapData+i)='0';
        }
    }

    //printf("mapdata: <%s>\n",fillWithMapData);
    free(filePath);

    int offX = x%MAP_WIDTH;
    int offY = y%MAP_HEIGHT;
    for (int i=0; i<3; i++) {
        *(returnTile+i)=*(fillWithMapData+(3*offY*MAP_WIDTH+3*offX)+i);
    }
    *(returnTile+3)='\0';
    free(fillWithMapData);
    int tileToInt = strtol(returnTile,NULL,36);
    free(returnTile);
    return tileToInt;
}

int tileWalkable(int tile) {
    printf("checking %d for walkability\n",tile);
    switch (tile) {
        case 11: //bricks (00b)
            return 0;
    }
    return 1;
}
