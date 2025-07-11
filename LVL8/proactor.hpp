#pragma once
#include <stdio.h>
#include <pthread.h>

// A function that takes a socket and handles it
typedef void* (*proactorFunc) (int sockfd);

// starts new proactor and returns proactor thread id.
pthread_t startProactor (int sockfd, proactorFunc threadFunc);

// stops proactor by threadid
int stopProactor(pthread_t tid);