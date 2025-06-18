#pragma once

#include <sys/select.h>

#define MAX 1024

typedef void*(*reactorFunc)(int fd); // function that handles certain client , reactor func gets int_fd and return void*
typedef struct reactor reactor;

// starts new reactor and returns pointer to it
reactor* startReactor ();

// adds fd to Reactor (for reading) ; returns 0 on success.
int addFdToReactor(reactor* reactor, int fd, reactorFunc func); 

// removes fd from reactor
int removeFdFromReactor(reactor* reactor, int fd);

// stops reactor
int stopReactor(reactor* reactor);

// runs select until he stops
void reactorRun(reactor* reactor);

void destroyReactor(reactor* reactor);