// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "general.h"

/**
 * This function creates a reliable socket with port reuse.
 * The socket file descriptor is returned on success, -1 otherwise
 * @param address sockaddr_in struct
 * @return the socket file descriptor on success, -1 on failure
 */
static int create_srv_socket(struct sockaddr_in *address){
    int fd = -1;
    int opt = 1;
    
    // Creating socket FD for IP based socket, over reliable link.
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket creation failed");
        return -1;
    }
    
    // Set the socket for multiple protocol support, address and port reuse
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0){
        perror("setsockopt");
        close(fd);
        return -1;
    }
    
    // Forcefully attaching socket to the port
    if(bind(fd, (struct sockaddr *) address, sizeof(struct sockaddr_in)) < 0){
        perror("bind failed");
        close(fd);
        return -1;
    }
    return fd;
}

ssize_t read_from_client (int filedes){
    char buffer[MAX_MSG_SIZE] = {0};
    ssize_t nbytes;
    
    nbytes = read (filedes, buffer, MAX_MSG_SIZE);
    if (nbytes < 0){
        /* Read error. */
        perror ("read");
        exit (EXIT_FAILURE);
    }
    else if (nbytes == 0){
    /* End-of-file. */
        return -1;
    }
    else{
        /* Data read. */
        fprintf (stderr, "Server: got message: '%s'\n", buffer);
        return nbytes;
    }
}

int main(int argc, char const *argv[]){
    ssize_t read_size;
    struct sockaddr_in address;
    int addrlen = 0;
    fd_set read_set;
    u_int8_t max_clients = 30;
    int master_fd, aux_fd, max_sd, client_sks[max_clients];
    // Timeout structures
    struct timeval timeout;
    
    // Clear file variables
    memset(&address, 0, sizeof(struct sockaddr_in));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SRV_PORT);
    
    addrlen = sizeof(address);
    
    if((master_fd = create_srv_socket(&address)) < 0){
        exit(EXIT_FAILURE);
    }
    
    if(listen(master_fd, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    while(1){
        // Block on read to wait for new input
        FD_ZERO(&read_set);
        FD_SET(master_fd, &read_set);
        
        // Define the timeout timer of each socket
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        
        max_sd = master_fd;
        
        // Add all clients to the set after zero
        for (int i = 0; i < max_clients; i++){
            aux_fd = client_sks[i];
            
            if (aux_fd > 0){
                FD_SET(aux_fd, &read_set);
            }
            if (aux_fd > max_sd){
                max_sd = aux_fd;
            }
        }
        
        // Wait for some activity in one of the sockets
        if(select(FD_SETSIZE, &read_set, NULL, NULL, &timeout) < 0){
            perror("select");
            close(master_fd);
            exit(EXIT_FAILURE);
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
        for (int i = 0; i <max_clients; i++){
            if(FD_ISSET(client_sks[i] ,&read_set)){
                if ((read_size = read_from_client(client_sks[i])) < 0){
                    close(client_sks[i]);
                    FD_CLR(i, &read_set);
                }
                else{
                    char *rsp = "Hello from server";
                    send(i , rsp , strlen(rsp) , 0 );
                    printf("Hello message sent\n");
                }
                close(client_sks[i]);
                client_sks[i] = 0;
                
            }
        }
    }
    
}
