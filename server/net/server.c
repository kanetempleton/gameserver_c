#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/mman.h>
#include "server.h"


Server * newServer() {
    return (Server *) malloc(sizeof(Server));
    //return (Server *)mmap(0,sizeof(Server),PROT_READ|PROT_WRITE, MAP_ANONYMOUS,-1,0);
}


void initServer(Server * me, int maxclients) {
    me->address = malloc(sizeof(struct sockaddr_in));
    me->opt = malloc(sizeof(int));
    me->master_socket = malloc(sizeof(int));
    me->addrlen = malloc(sizeof(int));
    me->new_socket = malloc(sizeof(int));
    me->activity = malloc(sizeof(int));
    me->i = malloc(sizeof(int));
    me->valread = malloc(sizeof(int));
    me->sd = malloc(sizeof(int));
    me->max_sd = malloc(sizeof(int));
    //me->client_socket=malloc(sizeof(int)*maxclients); //do something about client_socket
    me->max_clients = malloc(sizeof(int));
    //me->buffer = malloc(sizeof(char)*1025); //do something about buffer
    //me->sendbuffer = malloc(sizeof(char)*1025);//do something about sendbuffer
    me->readfds = malloc(sizeof(struct fd_set));

    me->nextSendTo=malloc(sizeof(int));
    *(me->nextSendTo)=-1;
    me->nextSendMsg=malloc(sizeof(char)*BUFFER_SIZE);

    /*int opt = 1;
    int master_socket,addrlen,new_socket,activity,i,valread,sd,max_sd,client_socket[30];
    int max_clients = 30;
    char buffer[1025];
    char sendbuffer[1025];
    fd_set readfds; //set of socket descriptors
    char *message = "free stuff pl0x\n";*/
}

void deleteServer(Server * me) {
    free(me->address);
    free(me->opt);
    free(me->master_socket);
    free(me->addrlen);
    free(me->new_socket);
    free(me->activity);
    free(me->i);
    free(me->valread);
    free(me->sd);
    free(me->max_sd);
    //free(me->client_socket);
    free(me->max_clients);
    //free(me->buffer);
    //free(me->sendbuffer);
    free(me->readfds);
    free(me->nextSendTo);
    free(me->nextSendMsg);
    //munmap(&me,sizeof(Server));
    free(me);
}


void messageToClient(Server * me, int id, char * m) {
    printf("messageToClient called\n");
    me->nextSendMsg=malloc(sizeof(char)*strlen(m)); //free this when it sends
    for (int i=0; i<strlen(m); i++) {
        *(me->nextSendMsg+i)=*(m+i);
    }
    *(me->nextSendTo)=id;
    printf("exiting messagetoclient with nextSendMsg=%s\n",me->nextSendMsg);
}

void resetSendBuffer(Server * me) {
    printf("resetting send buffer...\n");
    *(me->nextSendTo)=-1;
    free(me->nextSendMsg);
}


void * startServer(void * arg) {
    printf("startServer called\n");
    Server * me = (Server *)arg;
    //struct sockaddr_in address;
    *(me->opt) = 1;
    //int master_socket,addrlen,new_socket,activity,i,valread,sd,max_sd,client_socket[30];
    //int max_clients = 30;
    *(me->max_clients)=30;
    int client_socket[30];
    char buffer[1025];
    char sendbuffer[1025];
    //fd_set readfds; //set of socket descriptors
    char *message = "free stuff pl0x\n";
    int i;
    for (i=0; i< *(me->max_clients); i++) {
        client_socket[i]=0; //unchecked
    }

    if ((*(me->master_socket) = socket(AF_INET,SOCK_STREAM,0))==0) {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(*(me->master_socket),SOL_SOCKET,SO_REUSEADDR,(char *)&(*me->opt),sizeof(me->opt))<0) {
        perror("setsockopt failed.");
        exit(EXIT_FAILURE);
    } //allow multiple connections?


    (*me->address).sin_family = AF_INET;
    (*me->address).sin_addr.s_addr = INADDR_ANY;
    (*me->address).sin_port = htons(PORT);
    if ((*(me->address)).sin_family == AF_INET) {
        printf("true\n");
    }

    //bind socket
    if ((bind(*me->master_socket,(struct sockaddr*)&(*me->address),sizeof(*me->address)))<0) {
        perror("bind failed.");
        exit(EXIT_FAILURE);
    }

    if (listen(*(me->master_socket),3)<0) { //max of 3 pending connections
        perror("listen");
        exit(EXIT_FAILURE);
    }

    *(me->addrlen) = sizeof(*me->address);
    puts("Waiting for connections...");

    while (1) {
        FD_ZERO(&(*me->readfds)); //clear socket set
        FD_SET(*me->master_socket,&(*me->readfds)); //add master socket to set
        *(me->max_sd) = *(me->master_socket);

        for (i=0; i<*(me->max_clients); i++) {
            *(me->sd) = client_socket[i]; //socket descriptor
            if (*(me->sd)>0)
                FD_SET(*me->sd,&(*me->readfds)); //add valid socket descriptors to read list
            if (*(me->sd)>*(me->max_sd))
                *(me->max_sd)=*(me->sd);
            struct timeval tv;
            tv.tv_sec=0; //set selector delay to 0 to disable blocking
            *(me->activity) = select(*(me->max_sd)+1,&(*me->readfds),NULL,NULL,&tv);
            if (((*me->activity)<0) && (errno !=EINTR)) {
                printf("select error\n");
            }
        }

            if (FD_ISSET(*(me->master_socket),&(*(me->readfds)))) { //incoming connection
                if ((*(me->new_socket)=accept(*(me->master_socket),(struct sockaddr *)&(*(me->address)),(socklen_t*)&(*(me->addrlen))))<0) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , *(me->new_socket) , inet_ntoa((*(me->address)).sin_addr) , ntohs ((*(me->address)).sin_port));
                if (send(*(me->new_socket),message,strlen(message),0)!=strlen(message)) {
                    perror("send");
                }
                puts("Sent message");


                for (i=0; i<(*(me->max_clients)); i++) {
                    if (client_socket[i]==0) { //position empty
                        client_socket[i]=*(me->new_socket);
                        printf("Adding to list of sockets as %d\n",i);
                        break;
                    }
                }
            }
            /*if ((*(me->nextSendTo))>=0) {
                printf("flagged true\n");
                if (send(*(me->new_socket),me->nextSendMsg,strlen(me->nextSendMsg),0)!=strlen(me->nextSendMsg)) {
                    perror("sendbuf");
                }
                printf("resetting...");
                resetSendBuffer(me);//free shit
            }*/



            for (i=0; i<(*(me->max_clients)); i++) {
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
                if (FD_ISSET(*(me->sd),&(*(me->readfds)))) { //incoming messages.. i think?
                    puts("got something");
                    if ((*(me->valread) = read(*(me->sd),buffer,1024))==0) { //no this is person disconnecting
                        getpeername(*(me->sd),(struct sockaddr*)&(*(me->address)), \
                                    (socklen_t*)&(*(me->addrlen))); //TODO: lookup peername and the slash operator
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa((*(me->address)).sin_addr) , ntohs((*(me->address)).sin_port));   //TODO: ntohs
                        close(*(me->sd));
                        client_socket[i]=0;
                    }
                    else { //received new message
                        buffer[*(me->valread)] = '\0';
                        printf("<newmsg>%s</newmsg>\n",buffer);
                        send(*(me->sd),"testing response",strlen("testing response"),0); //send something else


                        //replyToSender()
                    }

                }
            }
    }
    return NULL;
}
