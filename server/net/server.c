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


char* sendText;
int sendTextTo;


Server * newServer() {
    return (Server *) malloc(sizeof(Server));
    //return (Server *)mmap(0,sizeof(Server),PROT_READ|PROT_WRITE, MAP_ANONYMOUS,-1,0);
}


void initServer(Server * me, int maxclients) {
    me->randomData = malloc(sizeof(int));
    me->userData = malloc(sizeof(EntryTable));
    me->userData->currentSlot = malloc(sizeof(int));
    *(me->userData->currentSlot) = 0;
    me->game = newGame();
    initGame(me->game);
    sendText = malloc(sizeof(char)*1024);
}

void deleteServer(Server * me) {
    free(me->randomData);
}


void messageToClient(int id, char * m) { //needs some force applied over a distance (work)
    printf("messageToClient called\n");
    sendTextTo = id;
    //sendText = m;
    strcpy(sendText,m);

    /*me->nextSendMsg=malloc(sizeof(char)*strlen(m)); //free this when it sends
    for (int i=0; i<strlen(m); i++) {
        *(me->nextSendMsg+i)=*(m+i);
    }
    *(me->nextSendTo)=id;
    printf("exiting messagetoclient with nextSendMsg=%s\n",me->nextSendMsg);*/
}

void resetSendBuffer(Server * me) {
    /*printf("resetting send buffer...\n");
    *(me->nextSendTo)=-1;
    free(me->nextSendMsg);*/
}

char * getUsername(Server* s, int fd) {
    return "titties";
}

void addUsername(Server* me, int from, char* user) {
    Entry* e = (Entry*)malloc(sizeof(Entry));
    e->name = user;
    e->id = malloc(sizeof(int));
    *(e->id) = from;
    me->userData->entries[*(me->userData->currentSlot)] = e;
    *(me->userData->currentSlot) = *(me->userData->currentSlot)+1;
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
    char *message = "free stuff pl0x\n";

    //create game thread
    //Game * game = newGame();
    //initGame(game);
    pthread_t gameThread;
    int rc = pthread_create(&gameThread,NULL,&runGame,(void*)me->game);
    //void * rval;
    //rc = pthread_join(gameThread, &rval);
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
            if (send(new_socket,message,strlen(message),0)!=strlen(message)) {
                perror("send");
            }
            puts("Sent message");

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
                    if ((valread = recv(sd,buffer,sizeof(buffer),0))==0) {
                        getpeername(sd,(struct sockaddr*)&address, \
                                    (socklen_t*)&addrlen); //TODO: lookup peername and the slash operator
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   //TODO: ntohs
                        close(sd);
                        client_socket[i]=0;

                    }
                    else { //send message
                        printf("valread:%d\n",valread);
                        buffer[valread] = '\0';
                        printf("<newmsg>%s</newmsg>\n",buffer);
                        //send(sd,"testing response",strlen("testing response"),0); //send something else
                        char* repl = getReply(me,sd,buffer);
                        if (strlen(repl)>0) {
                            printf("sending to %d\n",sd);
                            send(sd,repl,strlen(repl),0);
                        }
                        //replyToSender()
                    }
                }
            }
            if (sendTextTo>=0) {
                strcpy(sendbuffer,sendText);
                send(sendTextTo,sendbuffer,strlen(sendbuffer),0);
                //send(sendTextTo,sendbuffer,strlen(sendbuffer),0);
                //printf("sent %s\n",sendText);
                //handleReply(c->replyHandler,recvbuf);
                //clearBuffer(recvbuf);
                //sendText = "";
                memset(sendbuffer,0,sizeof(sendbuffer));
                sendTextTo=-1;
            }
            /*for (i=0; i<(*(me->max_clients)); i++) {
                *(me->sd) = client_socket[i];
                int yellsendto=-1;
                if (*(me->nextSendTo) != yellsendto) {
                    printf("nextSendTo: %d\n",*(me->nextSendTo));
                    yellsendto=*(me->nextSendTo);
                }
                if (( (*(me->nextSendTo))==client_socket[i]) && client_socket[i]!=0) {
                    printf("flagged true\n");
                    int sendcode=0;
                    sendcode = send(client_socket[i],me->nextSendMsg,strlen(me->nextSendMsg),0);
                    if (sendcode !=strlen(me->nextSendMsg)) {
                        perror("sendbuf");
                    } else {
                        printf("is working fuck nobody\n");
                        printf("send returned %d and nextsendmsg was length %lu\n",sendcode,strlen(me->nextSendMsg));
                        printf("and you tried to send %s to %d\n",me->nextSendMsg,*me->nextSendTo);
                    }
                    printf("resetting...");
                    resetSendBuffer(me);//free shit
                }
                if (FD_ISSET(*(me->sd),&(*(me->readfds)))) { //incoming messages
                    puts("got something");
                    if ((*(me->valread) = recv(*(me->sd),buffer,1024,0))==0) { //person disconnecting
                        getpeername(*(me->sd),(struct sockaddr*)&(*(me->address)), \
                                    (socklen_t*)&(*(me->addrlen))); //TODO: lookup peername and the slash operator
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa((*(me->address)).sin_addr) , ntohs((*(me->address)).sin_port));   //TODO: ntohs
                        close(*(me->sd));
                        client_socket[i]=0;
                    }
                    else { //received new message
                        //buffer[*(me->valread)] = '\0';
                        printf("<newmsg>%s</newmsg>\n",buffer);
                        //printf("reply:%s\n",getReply(buffer));
                        char* repl = getReply(buffer);
                        if (strlen(repl)>0)
                            send(*(me->sd),repl,strlen(repl),0); //reply to sender
                        // (*buffer='\0';


                    }
                    //buffer="";

                }
            }*/
    }
    return NULL;
}
