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

static ssize_t read_from_client (int filedes){
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
        fprintf (stderr, "Server: got message: '%s'\n", buffer);
        return nbytes;
    }
}


int server_create(){
    ssize_t read_size;
    
    int addrlen = 0, activity = -1;
    // Timeout structures
    struct sockaddr_in address;
    
    // Clear file variables
    memset(&address, 0, sizeof(struct sockaddr_in));
    
    // Listen socket configuration
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SRV_ADDR);
    address.sin_port = htons(0);
    addrlen = sizeof(address);
    
    if((master_fd = create_srv_socket(&address)) < 0){
        exit(EXIT_FAILURE);
    }
    
    fprintf(stdout, "Node listening on addr=%s, port= %d", SRV_ADDR, address.sin_port);
    
    if(listen(master_fd, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    
}
