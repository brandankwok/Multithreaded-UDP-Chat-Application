#ifndef _THREADS_H_
#define _THREADS_H_
#include "list.h"
#include <pthread.h>

void* keyboardThreadFunc(void* arg);
void keyboardInit(List* list);
void keyboardTerminate();

void* senderThreadFunc(void* arg);
void senderInit(List* list, char* destName, char* destPortNum);
void senderTerminate();

void* receiverThreadFunc(void* arg);
void receiverInit(List* list, char* myPortNum);
void receiverTerminate();

void* screenThreadFunc(void* arg);
void screenInit(List* list);
void screenTerminate();

#endif