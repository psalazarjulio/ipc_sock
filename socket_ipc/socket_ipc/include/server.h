//
//  main.h
//  ipc
//
//  Created by Pedro Salazar on 06/09/18.
//

#ifndef server_h
#define server_h

#include "general.h"

/*
 * Create an ipv4 network socket to listen at local_addr:port.
 * @param local_addr listening_address
 * @param local_port listening_port
 * @return the file descriptor of the new socket
 */
int server_create(char * local_addr, int local_port);

/*
 * Read data from client on file descriptor
 *
 */
ssize_t server_read_from_client (int filedes);

#endif /* main_h */
