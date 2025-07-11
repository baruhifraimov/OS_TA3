#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "reactor.hpp"

using namespace std;

struct reactor {
	fd_set client_fds; // array of fd's
	reactorFunc reactorFuncs[MAX]; 	// array of functions
	int maxfd; 						// highest value fd in the array
	int running;					// flag for controling the loop
};

reactor* startReactor() {
    reactor *r = (reactor *)malloc(sizeof(reactor)); //creating a new reactor and Allocate memory for the reactor struct dynamically
    if (!r) return NULL;
    FD_ZERO(&r->client_fds); //initializing fd_set to 0 - to use select after
    memset(r->reactorFuncs, 0, sizeof(r->reactorFuncs)); //sets all the function array cells to null.
    r->maxfd = -1; // sets max fd to -1 , because we dont have one yet.
    r->running = 1; // the reactor is now working.   
    return r; //return the reactor.
}

int addFdToReactor(reactor* reactor, int fd, reactorFunc func){
    if(!reactor){ // if reactor is not set the return -1;
		return -1;
	}

	FD_SET(fd, &reactor->client_fds); // set fd to be one on the clients_fd
	reactor->reactorFuncs[fd] = func; //add the function that we got to be this fd function to be use.

	if(reactor->maxfd < fd){ //updating maxfd if our fd is the max. 
		reactor->maxfd = fd;
	}

	return 0;
}

int removeFdFromReactor(reactor* reactor, int fd){
	if(!reactor){   // if reactor is not set the return -1;
		return -1;
	}

	FD_CLR(fd, &reactor->client_fds); //removing our fd from clients_fd
	reactor->reactorFuncs[fd] = nullptr; //removing it function as well.
	
	// if the curreent maxfd is the same fd we want to remove
	// picks the next highest fd as maxfd
	if(reactor->maxfd == fd){
		int new_max_fd = -1;
		for (int i = fd-1; i >= 0; i--)
		{
			if (FD_ISSET(i, &reactor->client_fds)){
				new_max_fd = i;
				break;
			}
		}
		reactor->maxfd = new_max_fd;
	}

	return 0;
}

int stopReactor(reactor* reactor){
	// check if not null
	if (!reactor){
		return -1;
	}

	// set flag to NOT
	reactor->running = 0;
    return 0;
}

void reactorRun(reactor* reactor){
	while (reactor->running){
        fd_set read_fds;
        FD_ZERO(&read_fds); // clear the read_fds set
        read_fds = reactor -> client_fds; // copy client's fd set into read_fds
		
		 // wait for activity on one of the file descriptors
		int choose = select(reactor->maxfd+1 ,&read_fds, NULL, NULL, NULL); 
        if (choose <= 0){ // not alive/timeout, handle it
            if (errno == EINTR) 	// interrupted by signal — safe to retry
                continue;
            perror("select"); // some other error
            break;
        }

		// fd is alive, check if is set and if he got a function, then run the function on it
        for(int fd=0; fd<= reactor->maxfd;fd++){
            if (FD_ISSET(fd,&read_fds) && reactor->reactorFuncs[fd]){
                reactor->reactorFuncs[fd](fd);
            }
        }
    }
}

void destroyReactor(reactor* reactor){
	if (!reactor){
		return;
	}
	free(reactor);
}