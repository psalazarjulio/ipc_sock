//
//  mainloop.c
//  socket_ipc
//
//  Created by Pedro Salazar on 17/09/18.
//  Copyright © 2018 me. All rights reserved.
//

#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "general.h"

int mainloop(int master_fd, ){
    
    u_int8_t max_clients = 30;
    fd_set read_set;
    struct timeval timeout;
    int aux_fd, maxfd, client_sks[max_clients], activity;
    
    
    while(1){
        // Block on read to wait for new input
        FD_ZERO(&read_set);
        FD_SET(master_fd, &read_set);
        activity = -1;
        
        // Define the timeout timer of each socket
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        
        maxfd = master_fd;
        
        // Add all clients to the set after zero
        for (int i = 0; i < max_clients; i++){
            aux_fd = client_sks[i];
            
            if (aux_fd > 0){
                FD_SET(aux_fd, &read_set);
            }
            if (aux_fd > maxfd){
                maxfd = aux_fd;
            }
        }
        
        // Wait for some activity in one of the sockets
        activity = select(maxfd + 1, &read_set, NULL, NULL, &timeout);
        
        if (activity < 0 && errno == EINTR){
            continue;
        }
        
        if(activity < 0){
            perror("select");
            close(master_fd);
            exit(EXIT_FAILURE);
        }
        else if(activity == 0){
            
        }
        
        // Handle incoming connetions
        if(FD_ISSET(master_fd, &read_set)){
            int new_socket;
            
            if ((new_socket = accept(master_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
                perror("accept");
                exit(EXIT_FAILURE);
            }
            fprintf (stderr, "Server: connection from host=%s, port= %hd.\n", inet_ntoa (address.sin_addr), ntohs(address.sin_port));
            
            for (int i=0; i<max_clients; i++){
                if(client_sks[i] == 0){
                    client_sks[i] = new_socket;
                    break;
                }
            }
        }
        // Look for data in other sockets
        for (int i = 0; i < max_clients; i++){
            if(FD_ISSET(client_sks[i] ,&read_set)){
                if ((read_size = read_from_client(client_sks[i])) <= 0){
                    printf("Server: Client %d terminated connection", i);
                    close(client_sks[i]);
                    FD_CLR(i, &read_set);
                    client_sks[i] = 0;
                }
                else{
                    char *rsp = "Hello from server";
                    write(client_sks[i] , rsp , strlen(rsp));
                    printf("Hello message sent\n");
                }
            }
        }
    }
    
}
