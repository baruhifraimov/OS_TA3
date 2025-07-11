#include "proactor.hpp"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct proactor
{
	int fd;
	proactorFunc func;
}proactor;


//pack sockfd and func into a proactor (a struct) to be able to syncroniozed with the thread creation standart.
void* funcWrapper(void* args){ 
	proactor* pactr = (proactor*)args;
	void* result = (pactr->func)(pactr->fd);
	free (pactr);
	return result;
}

// starts new proactor and returns proactor thread id.
pthread_t startProactor (int sockfd, proactorFunc threadFunc){
	proactor* pactr = (proactor*)malloc(sizeof(proactor));

	if(!pactr){
		perror("malloc");
		exit(0);
	}

	pactr->fd = sockfd;
	pactr->func = threadFunc;

	pthread_t thread;
	if (pthread_create(&thread, NULL, funcWrapper, pactr) != 0 ){
		perror("pthread_create");
		close(sockfd);
		free(pactr);
		return 0;
	}

	pthread_detach(thread);
	printf("Proactor started for socket %d with thread ID %lu\n", sockfd, thread);
	return thread;
}

// stops proactor by threadid
int stopProactor(pthread_t tid) {
    return pthread_cancel(tid); // simple option to stop a thread.
}