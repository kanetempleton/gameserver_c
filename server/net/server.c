#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/mman.h>
#include "server.h"
#include "communication/ReplyHandler.h"
#include "../game/game.h"
#include "communication/Codes.h"
#include <pthread.h>

pthread_mutex_t serverLock;



char* sendText; //i feel like i shouldn't declare these here
int sendTextTo;
int broadcastTextFlag;
char* broadcastText;
int dontBroadcastTo;
int kick;


Server * newServer() {
    return (Server *) malloc(sizeof(Server));
    //return (Server *)mmap(0,sizeof(Server),PROT_READ|PROT_WRITE, MAP_ANONYMOUS,-1,0);
}


void initServer(Server * me, int maxclients) {
    pthread_mutex_init(&serverLock,NULL);
    mainGame = newGame();
    initGame(mainGame);
    sendText = malloc(sizeof(char)*1024);
    broadcastText = malloc(sizeof(char)*1024);
    kick=-1;
    broadcastTextFlag = 0;
    dontBroadcastTo = -1;
}


void deleteServer(Server * me) {
    deleteGame(mainGame);
}


void messageToClient(int id, char * m) { //needs some force applied over a distance (work)
    pthread_mutex_lock(&serverLock);
    sendTextTo = id;
    //sendText = m;
    //strcpy(sendText,m);
    send(sendTextTo,m,strlen(m),0);
    //send(sendTextTo,PADDED_HALT,strlen(PADDED_HALT),0);
    printf("sent <%s>\n",m);
    pthread_mutex_unlock(&serverLock);
}


char * getUsername(Server* s, int fd) {
    return ""; //unfinished
}

void messageToAll(char* m, int ds) {
    strcpy(broadcastText,m);
    broadcastTextFlag = 1;
    if (ds!=-1)
        dontBroadcastTo=ds;
}


void * startServer(void * arg) {
    printf("startServer called\n");
    Server * me = (Server *)arg;
    struct sockaddr_in address;
    int opt = 1;
    int master_socket,addrlen,new_socket,activity,valread,sd,max_sd,client_socket[30];
    int max_clients = 30;
    int connectedClients = 0;
    char buffer[1025];
    char sendbuffer[1025];
    fd_set readfds; //set of socket descriptors

    pthread_t gameThread; //create game thread
    int rc = pthread_create(&gameThread,NULL,&runGame,(void*)mainGame);

    int i;
    for (i=0; i< max_clients; i++) {
        client_socket[i]=0; //unchecked
    }

    if ((master_socket = socket(AF_INET,SOCK_STREAM,0))==0) {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))<0) {
        perror("setsockopt failed.");
        exit(EXIT_FAILURE);
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //bind socket
    if (bind(master_socket,(struct sockaddr*)&address,sizeof(address))<0) {
        perror("bind failed.");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket,3)<0) { //max of 3 pending connections
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Waiting for connections...");

    while (1) {
        FD_ZERO(&readfds); //clear socket set
        FD_SET(master_socket,&readfds); //add master socket to set
        max_sd = master_socket;

        for (i=0; i<max_clients; i++) {
            sd = client_socket[i]; //socket descriptor
            if (sd>0)
                FD_SET(sd,&readfds); //add valid socket descriptors to read list
            if (sd>max_sd)
                max_sd=sd;
            struct timeval tv;
            tv.tv_sec=0; //set selector delay to 0 to disable blocking
            activity = select(max_sd+1,&readfds,NULL,NULL,&tv);
            if ((activity<0) && (errno!=EINTR)) {
                printf("select error\n");
            }

        }

        if (FD_ISSET(master_socket,&readfds)) { //incoming connection
            if ((new_socket=accept(master_socket,(struct sockaddr *)&address,(socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));

            for (i=0; i<max_clients; i++) {
                if (client_socket[i]==0) { //position empty
                    client_socket[i]=new_socket;
                    printf("Adding to list of sockets as %d\n",i);
                    break;
                }
            }
        }

        for (int i=0; i<max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd,&readfds)) {
                puts("got something");
                if ((valread = recv(sd,buffer,sizeof(buffer),0))==0) { //disconnection
                    getpeername(sd,(struct sockaddr*)&address, \
                                (socklen_t*)&addrlen); //TODO: lookup peername and the slash operator
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   //TODO: ntohs
                    Player* ppp = getPlayer_fd(mainGame,sd);
                    if (ppp!=NULL) {
                        printf("player was %s with id %d\n",ppp->playerName,*ppp->playerId);
                        removePlayer(mainGame,*ppp->playerId);
                    }
                    close(sd);
                    client_socket[i]=0;

                }
                else { //received a new message

                    buffer[valread] = '\0';
                    printf("<newmsg>%s</newmsg>\n",buffer);

                    char* repl = getReply(me,sd,buffer); //send a reply
                    if (strcmp(repl,"BAD")==0) {
                        printf("removing client\n");
                        close(sd);
                        client_socket[i]=0;
                    }
                    if (strlen(repl)>0) {
                        printf("sending to %d\n",sd);
                        send(sd,repl,strlen(repl),0);
                    }
                    //replyToSender()
                }
            }
        }
        /*if (broadcastTextFlag==1) {
            printf("broadcasting; dontsend to %d\n",dontBroadcastTo);
            for (int i=0; i<*(mainGame->nextPlayerId); i++) {
                strcpy(sendbuffer,broadcastText);
                printf("getting the fd \n");
                Player* p = getPlayer(mainGame,i);
                if (p==NULL)
                    continue;
                int fd = *(p->playerFd);
                printf("doing a check\n");
                if (fd!=dontBroadcastTo || dontBroadcastTo==-1) {
                    printf("going to %d \n",fd);
                    send(fd,sendbuffer,strlen(sendbuffer),0);
                }
                printf("bout2memset \n");
                memset(sendbuffer,0,sizeof(sendbuffer));
            }
            printf("done resetting flags \n");
            broadcastTextFlag=0;
            dontBroadcastTo = -1;
        }*/
        /*if (sendTextTo>=0) { //general text sending using global fields
            strcpy(sendbuffer,sendText);
            send(sendTextTo,sendbuffer,strlen(sendbuffer),0);
            memset(sendbuffer,0,sizeof(sendbuffer));
            sendTextTo=-1;
        }*/


    }
    return NULL;
}
