#ifndef ADAM6X50_THREAD_H
#define ADAM6X50_THREAD_H

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "adam6x50_conf.h"
#include "../cqueue/cqueue.h"
#include "../datastruct/adamtag.h"
#include <unistd.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 1025   //The port on which to listen for incoming data

class adam6x50_thread
{
private :
	string sConnectionString;
	cqueue<adamtag> *cqReadToPLC;
	cqueue<adamtag> *cqResponseFromPLC;
	cqueue<adamtag> *cqWriteToPLC;
	
	pthread_t thread_id;
	void threadFunction(void* args);
	
	struct threadArgsStruct 
	{
		char *cConnectionString;
		cqueue<adamtag> *cqReadToPLC;
		cqueue<adamtag> *cqResponseFromPLC;
		cqueue<adamtag> *cqWriteToPLC;
	} thargs;

public:
	void SetConnectionString(string sConnectionString);	
	void SetQueueAdamReadToPLC(cqueue<adamtag> *cqReadToPLC);
	void SetQueueAdamResponseFromPLC(cqueue<adamtag> *cqResponseFromPLC);
	void SetQueueAdamWriteToPLC(cqueue<adamtag> *cqWriteToPLC);
	static void *ThreadWrapper(void *thisPtr)
	{
		((adam6x50_thread*) thisPtr)->threadFunction(thisPtr);
		return NULL;
	}
	
	void ThreadStart();
	void ThreadJoin();
};

//TODO function STOP + JOIN + Passage pointeur queue

#endif // ADAM6X50_THREAD_H
