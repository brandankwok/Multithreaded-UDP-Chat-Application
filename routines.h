#ifndef _ROUTINES_H_
#define _ROUTINES_H_
#include <pthread.h>
#include "list.h"

void freeMessage(void* message);
void addMessage(char* message, List* list, pthread_cond_t* empty);
char* takeMessage(List* list, pthread_cond_t* empty);

#endif
