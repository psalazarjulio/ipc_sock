// Server side C/C++ program to demonstrate Socket programming

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
#include "server.h"

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

ssize_t server_read_from_client (int filedes){
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
        return nbytes;
    }
    else{
        /* Data read. */
        fprintf (stderr, "Got message: '%s'\n", buffer);
        return nbytes;
    }
}

int server_create(char * local_addr, int local_port){
    
    int addrlen = 0, fd = -1;
    struct sockaddr_in address, aux_address;
    unsigned int aux_address_len = sizeof(aux_address);
    char my_addr[16];
    
    // Clear file variables
    memset(&address, 0, sizeof(struct sockaddr_in));
    
    // Listen socket configuration
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // TODO: Listen on any port
    address.sin_port = htons(0);         //
    addrlen = sizeof(address);
    
    if((fd = create_srv_socket(&address)) < 0){
        fprintf(stdout, "Unable to create socket");
        exit(EXIT_FAILURE);
    }
    
    if(getsockname(fd, (struct sockaddr *) &aux_address , &aux_address_len)){
        perror("getsockname");
    }
    inet_ntop(AF_INET, &aux_address.sin_addr, my_addr, sizeof(my_addr));
    printf("Node listening on addr=%s, port=%d\n", my_addr, ntohs(aux_address.sin_port));
    
    if(listen(fd, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return fd;
}
