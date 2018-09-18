//
//  client.c
//  ipc
//
//  Created by Pedro Salazar on 06/09/18.
//
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "general.h"
#include "client.h"


static int client_establish_remote_connection(struct sockaddr_in * address, int sock){
    
    int max_retry = 8, current_retry = 1, factor = 2;
    
    while (current_retry < max_retry){
        if ((connect(sock, (struct sockaddr *)&address, sizeof(address))) < 0){
            perror("Connect");
            sleep(current_retry);
            current_retry *= factor;
        }
        else{
            return 0;
        }
    }
    return -1;
}

int client_connect_to_remote_node(char * remote_host_address, int remote_host_port){
    
    //struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *snd_str = "Hello from client";
    
    // Creating socket FD for IP based socket, over reliable link.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(remote_host_port);
    serv_addr.sin_addr.s_addr = inet_addr(remote_host_address);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, SRV_ADDR, &serv_addr.sin_addr) <=0 ){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if(client_establish_remote_connection(&serv_addr, sock) < 0){
        return -1;
    }
    
    write(sock , snd_str , strlen(snd_str));
    printf("Hello message sent\n");
    
    return sock;
}

