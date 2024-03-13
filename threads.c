#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "list.h"
#include "threads.h"
#include "routines.h"

static List* senderList;
static List* receiverList;

pthread_cond_t senderEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t receiverEmpty = PTHREAD_COND_INITIALIZER;

pthread_t keyboardThreadPID;
pthread_t senderThreadPID;
pthread_t receiverThreadPID;
pthread_t screenThreadPID;

static int socketDescriptorSend;
struct addrinfo *result, *current; 
static int socketDescriptorReceive;

#define MAX_MSG_LEN 256

// THREAD TERMINATION FUNCTIONS 

void keyboardTerminate() {
    pthread_cancel(keyboardThreadPID);
    pthread_join(keyboardThreadPID, NULL);
}
void senderTerminate() {
    pthread_cancel(senderThreadPID);
    pthread_join(senderThreadPID, NULL);
}
void receiverTerminate() {
    pthread_cancel(receiverThreadPID);
    pthread_join(receiverThreadPID, NULL);
}
void screenTerminate() {
    pthread_cancel(screenThreadPID);
    pthread_join(screenThreadPID, NULL);
}

// THREAD ROUTINES

void* keyboardThreadFunc(void* arg) {
    while(1) {
        // allocate memory for input
        char* input = (char*) malloc (MAX_MSG_LEN * sizeof(char));
        if (input == NULL) {
            return NULL;
        }
        //check if threads can access main thread info after it ends
        if (fgets(input, MAX_MSG_LEN, stdin) == NULL) {
            printf("Error reading input\n");
            exit(1);
            return NULL; 
        }
        // replace newline char with null terminating char
        int length = strlen(input)-1;
        input[length] = 0;
        // add message input to the list
        addMessage(input, senderList, &senderEmpty); 
        // check for ! and terminate appropriate threads
        if (input[0] == '!' && length == 1) {
            printf("User typed !, ending s-talk...\n");
            receiverTerminate();
            screenTerminate();
            pthread_exit(NULL);
        }
    }
    return NULL;
}

void* senderThreadFunc(void* arg) {
    while (1) {
        char* message;
        // get message from the sender list
        message = takeMessage(senderList, &senderEmpty); 
        if (message == NULL) {
            printf("Message could not be retrieved\n");
            return NULL;
        }
        int messageLength = strlen(message);
        // send message to destination, returns -1 if error
        if (sendto(socketDescriptorSend, message, messageLength, 0, current->ai_addr, current->ai_addrlen) == -1) {
            printf("could not send message\n");
            return NULL;
        }
        // check for ! and close socket and terminate thread
        if (message[0] == '!' && messageLength == 1) {
            close(socketDescriptorSend);
            close(socketDescriptorReceive);
            pthread_exit(NULL);
            return NULL;
        }
    }
    return NULL;
}

void* receiverThreadFunc(void* arg) {
    struct sockaddr_in receivedFrom; 
    unsigned int addrLen = sizeof(receivedFrom);
    //buffer to fill in message
    char buffer[MAX_MSG_LEN];
    
    while (1) {
        // receives message to destination, returns -1 if error
        int numOfBytes = recvfrom(socketDescriptorReceive, buffer, MAX_MSG_LEN - 1, 0, (struct sockaddr*)&receivedFrom, &addrLen);
        if (numOfBytes == -1) {
            printf("error: could not receive");
            exit(1);
        }
        // set up the buffer
        if (numOfBytes == MAX_MSG_LEN) {
            numOfBytes--; 
        }
        buffer[numOfBytes] = 0; 
        // add message to receiverList
        addMessage(buffer, receiverList, &receiverEmpty);
        // check for ! and close socket and terminate appropriate threads
        if (!strcmp(buffer, "!")) { 
            close(socketDescriptorReceive);
            close(socketDescriptorSend);
            senderTerminate();
            keyboardTerminate();
            pthread_exit(NULL);
            return NULL;
        }
    }
    return NULL;
}

void* screenThreadFunc(void* arg) {
    char* output;
    while(1) {
        // retrieve last item in list (earliest message)
        output = takeMessage(receiverList, &receiverEmpty);
        if (output == NULL) {
            printf("Message could not be retrieved\n");
            return NULL;
        }
        // check if output is ! and exit the thread
        if (!strcmp(output, "!")) {
            printf("Received !, ending s-talk...\n");
            pthread_exit(NULL);
        }
        // print message to screen
        if (write(1, output, strlen(output)) == -1) {
            printf("Message could not be printed to screen\n");
            return NULL;
        }
        printf(" (from remote client)\n");
    }
    return NULL;
}

// THREAD INITIALIZER FUNCTIONS

void keyboardInit(List* list) {
    senderList = list;

    if (pthread_create(&keyboardThreadPID, NULL, keyboardThreadFunc, NULL) != 0) {
        perror("Could not create keyboard thread");
        exit(1);
    }
}

void senderInit(List* list, char* destName, char* destPortNum) {
    senderList = list;
    //socket criteria struct
    struct addrinfo sin; 
    memset(&sin, 0, sizeof sin); 
    sin.ai_family = AF_INET; 
    sin.ai_socktype = SOCK_DGRAM;
    
    int status = getaddrinfo(destName, destPortNum, &sin, &result);
    if(status != 0) {
        printf("could not get addrinfo\n");
        return;
    }
    // socket() creates an int that will be associated with a socket 
    current = result;
    while (current != NULL) {
        // protocol should be 0
        socketDescriptorSend = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (socketDescriptorSend == -1) {
            current = current->ai_next; 
            continue; 
        }
        break;
    }
    if (current == NULL) {
        //program fails (no sockets to bind to)
        printf("not sockets to get, exiting\n");
    }
    //start the sender thread
    if (pthread_create(&senderThreadPID, NULL, &senderThreadFunc, NULL) != 0) {
        printf("Could not create sender thread");
        exit(1);
    }
    freeaddrinfo(result);
}

void receiverInit(List* list, char* myPortNum) {
    receiverList = list;
    //socket criteria struct
    struct sockaddr_in sin; 
    memset(&sin, 0, sizeof(sin)); 
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(atoi(myPortNum));
    // create and bind socket
    socketDescriptorReceive = socket(PF_INET, SOCK_DGRAM, 0);
    bind(socketDescriptorReceive, (struct sockaddr*) &sin, sizeof(sin));

    if (pthread_create(&receiverThreadPID, NULL, &receiverThreadFunc, (void*) list) != 0) {
        printf("Could not create receiver thread");
        exit(1);
    }
}

void screenInit(List* list) {
    receiverList = list;

    if (pthread_create(&screenThreadPID, NULL, screenThreadFunc, NULL) != 0) {
        perror("Could not create screen thread");
        exit(1);
    }
}

