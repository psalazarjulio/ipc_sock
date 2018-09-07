//
//  client.c
//  ipc
//
//  Created by Pedro Salazar on 06/09/18.
//

#include "general.h"


// Client side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
    //struct sockaddr_in address;
    int sock = 0;
    ssize_t read_size;
    struct sockaddr_in serv_addr;
    char *snd_str = "Hello from client";
    char buffer[MAX_MSG_SIZE] = {0};
    
    
    // Creating socket FD for IP based socket, over reliable link.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SRV_PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, SRV_ADDR, &serv_addr.sin_addr) <=0 ){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }
    
    send(sock , snd_str , strlen(snd_str) , 0);
    printf("Hello message sent\n");
    
    read_size = read(sock , buffer, 1024);
    printf("%s\n",buffer );
    
    close(sock);
    
    return 0;
}
