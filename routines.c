#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"
#include "threads.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

void freeMessage(void* message) {
    free(message);
}

//NOTE: use prepend and trim for a queue (prepend adds to front, trim takes from back)
void addMessage(char* message, List* list, pthread_cond_t* empty) {
    // lock mutex
    pthread_mutex_lock(&mutex);
    // wait if list is full
    while (List_prepend(list, (void*) message) == -1) {
        pthread_cond_wait(&full, &mutex); 
    } 
    pthread_cond_signal(empty); 
    pthread_mutex_unlock(&mutex); 
}

char* takeMessage(List* list, pthread_cond_t* empty) {
    // lock mutex
    pthread_mutex_lock(&mutex); 
    // wait if the list has no items
    if (List_count(list) == 0) {
        pthread_cond_wait(empty, &mutex);
      
    }
    // retrieve the message
    char* message = (char*) List_trim(list); 
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mutex); 
    return message;
}