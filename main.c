#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "list.h"
#include "threads.h"
#include "routines.h"


int main (int argc, char** argv) {
    // checking for incorrect number of arguments
    if (argc != 4) {
        printf("User provided too many or too little arguments");
        return 1;
    }
    // setting arguments
    char* myPortNum = argv[1];
    char* destName = argv[2];
    char* destPortNum = argv[3];
    // create two lists for the threads
    List* senderList = List_create();
    List* receiverList = List_create();

    printf("Starting s-talk...\nPress ! to end s-talk\n\n");
    // start threads
    senderInit(senderList, destName, destPortNum);
    receiverInit(receiverList, myPortNum);
    screenInit(receiverList);
    keyboardInit(senderList); 
    // free lists and exit main thread
    List_free(senderList, freeMessage);
    List_free(receiverList, freeMessage);
    pthread_exit(NULL);
    return 0;
}



