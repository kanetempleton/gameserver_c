/*  Database.c
    Handles login requests by checking if file exists for a given username
    and determining if passwords match
*/

#include "Database.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int loginCheck(char* user, char* pass) {
    char * filePath = malloc(sizeof(char)*strlen(user)+sizeof(char)*strlen("data/login/.txt"));
    strcpy(filePath,"data/login/");
    strcat(filePath,user);
    strcat(filePath,".txt");
    if( access( filePath, F_OK ) != -1 ) { //file exists so read from it
        printf("file exists\n");
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
                while( token != NULL ) {
                    if (t==0 && lineNum==0) { // check password
                        char* token2=strtok(token,"\n");
                        if (strcmp(token2,pass)==0) {
                            printf("correct password\n");
                            return LOGIN_SUCCESS;
                        }
                        else {
                            printf("invalid password\n");
                            return LOGIN_INVALID;
                        }
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
    } else { //File does not exist - create it
        printf("file DNE\n");
        FILE * fp;
        fp = fopen(filePath,"a");
        //for (int i=0; i<1; i++) {
        fprintf(fp,"%s",pass);
        //}
        fprintf(fp,"\nEOF");
        fclose(fp);
        free(filePath);
        return LOGIN_NEW;
    }
    free(filePath);
    /*if (strcmp(user,"admin")==0) {
        if (strcmp(pass,"test")==0)
            return LOGIN_SUCCESS;
        else
            return LOGIN_INVALID;
    }*/
    return LOGIN_INVALID;
}
