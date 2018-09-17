//
//  main.c
//  socket_ipc
//
//  Created by Pedro Salazar on 17/09/18.
//  Copyright © 2018 me. All rights reserved.
//
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "general.h"

void usage(){
    fprintf(stdout, "Show usage message\n");
    fprintf(stdout, "\n");
}

int main(int argc, char ** argv){
    int c, remote_port;
    char remote_addr[] = SRV_ADDR;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, "ap")) != -1)
        switch (c)
    {
        case 'p':
            remote_port = atoi(optarg);
            break;
        case 'a':
            strcpy(remote_addr, optarg);
            break;
        case '?':
            if (optopt == 'c')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,"Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            usage();
            exit(1);
    }
    
    // Create server
    
    
    // Create client
    
}

