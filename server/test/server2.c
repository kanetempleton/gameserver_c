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
#include "server2.h"

#define PORT 10000


Server2* newServer2() {
    return (Server2*)malloc(sizeof(Server2));
}
void initServer2(Server2* s) {
    s->sendMessage = malloc(sizeof(char));
    *(s->sendMessage) = '\0';
    s->sendTo = malloc(sizeof(int));
    *(s->sendTo) = 0;
}
void deleteServer2(Server2* s) {
    free(s->sendMessage);
    free(s->sendTo);
    free(s);
}
void messageToClient2(Server2* s, int l, char* send) {
    printf("trolololol\n");
}
void * startServer2(void * arg) {
    Server2* s = (Server2*)arg;
    int opt = 1;
    int master_socket,addrlen,new_socket,activity,valread,sd,max_sd,client_socket[30];
    int max_clients = 30;
    char buffer[1025];
    char sendbuffer[1025];
    fd_set readfds; //set of socket descriptors
    char *message = "free stuff pl0x\n";
    struct sockaddr_in address;


    for (int i=0; i< max_clients; i++) {
        client_socket[i]=0; //initialize client sockets
    }

    if ((master_socket = socket(AF_INET,SOCK_STREAM,0))==0) {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt))<0) {
        perror("setsockopt failed.");
        exit(EXIT_FAILURE);
    } //allow multiple connections?

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

        for (int i=0; i<max_clients; i++) {
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

                for (int i=0; i<max_clients; i++) {
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
                    if ((valread = read(sd,buffer,1024))==0) {
                        getpeername(sd,(struct sockaddr*)&address, \
                                    (socklen_t*)&addrlen); //TODO: lookup peername and the slash operator
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   //TODO: ntohs
                        close(sd);
                        client_socket[i]=0;

                    }
                    else { //send message
                        buffer[valread] = '\0';
                        printf("<newmsg>%s</newmsg>\n",buffer);
                        send(sd,"testing response",strlen("testing response"),0); //send something else
                        //replyToSender()
                    }
                }
            }
    }
}
