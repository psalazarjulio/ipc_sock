//
//  connection_manager.h
//  socket_ipc
//
//  Created by Pedro Salazar on 19/09/18.
//  Copyright © 2018 me. All rights reserved.
//

#ifndef connection_manager_h
#define connection_manager_h

#include <stdio.h>

int connection_manager_create_remote_connection(char * remote_host_address, int remote_host_port);

/*
 * Create an ipv4 network socket to listen at local_addr:port.
 * @param local_addr listening_address
 * @param local_port listening_port
 * @return the file descriptor of the new socket
 */
int connection_manager_create_listner(char * local_addr, int local_port);

/*
 * Read data from client on file descriptor
 *
 */
    ssize_t connection_manager_read_from_peer(int filedes);


#endif /* connection_manager_h */
