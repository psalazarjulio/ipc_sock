//
//  main.c
//  socket_ipc
//
//  Created by Pedro Salazar on 17/09/18.
//  Copyright © 2018 me. All rights reserved.
//
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "general.h"
#include "server.h"
#include "client.h"

static void usage(char * funcname){
    fprintf(stderr, "Usage: %s -a <address_to_connect> -p <port_connect>\n", funcname);
    fprintf(stderr, "\n");
    fprintf(stderr, "\t-p\t\t port number of remote host\n");
    fprintf(stderr, "\t-a\t\t address of remote host\n");
    fprintf(stderr, "\n");
}

static void mainloop(int master_fd, char * remote_host_address, int remote_host_port){
    
    ssize_t read_size;
    fd_set read_set;
    unsigned int max_clients = 16;
    struct timeval timeout;
    int aux_fd, maxfd, parent_sk = 0, client_sks[max_clients], activity = -1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if (remote_host_port > 0){
        fprintf(stdout, "Configuring client \n ");
        parent_sk = client_connect_to_remote_node(remote_host_address, remote_host_port);
    }
    else{
        fprintf(stdout, "Client connection not started by default\n");
    }
    
    // Zero structures
    memset(client_sks, 0, sizeof(client_sks));
    
    // operation loop
    while(1){
        // Block on read to wait for new input
        FD_ZERO(&read_set);
        FD_SET(master_fd, &read_set);
        if(parent_sk > 0 ){
            FD_SET(parent_sk, &read_set);
        }
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
            // define behaviour
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
                if ((read_size = server_read_from_client(client_sks[i])) <= 0){
                    printf("Server: Client %d terminated connection\n", i);
                    close(client_sks[i]);
                    FD_CLR(i, &read_set);
                    client_sks[i] = 0;
                }
                else{
                    char *rsp = "Hello from server\n";
                    write(client_sks[i] , rsp , strlen(rsp));
                    printf("Hello message sent\n");
                }
            }
        }
    }
    
}


int main(int argc, char ** argv){
    int c, remote_port = 0, local_socket;
    char remote_addr[] = SRV_ADDR;
    
    opterr = 0;
    
    printf("Starting Node\n");
    
    while ((c = getopt (argc, argv, "a:p:")) != -1){
        switch (c){
            case 'p':
                remote_port = atoi(optarg);
                break;
            case 'a':
                strcpy(remote_addr, optarg);
                break;
            case '?':
                if (optopt == 'p'){
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    usage(argv[0]);
                    exit(1);
                }
                else if (optopt == 'a'){
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    usage(argv[0]);
                    exit(1);
                }
                else if (isprint (optopt)){
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                    usage(argv[0]);
                    exit(1);
                }
                else{
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                    usage(argv[0]);
                    exit(1);
                }
            default:
                usage(argv[0]);
                exit(1);
        }
    }
    
    // Create server
    local_socket = server_create(SRV_ADDR, 0);
    
    // Create control socket
    
    // Start execution
    if(local_socket > 0)
        mainloop(local_socket, remote_addr, remote_port);
    
}


