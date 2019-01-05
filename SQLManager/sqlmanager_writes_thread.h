#ifndef SQLMANAGER_WRITES_THREAD_H
#define SQLMANAGER_WRITES_THREAD_H
#include "../shared.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <errno.h>
#include "../cqueue/cqueue.h"
#include "../datastruct/datatag.h"
#include <postgresql/libpq-fe.h>
#include <list>

class sqlmanager_writes_thread
{
private :
	string sConnectionString;
	cqueue<datatag> *cqWriteRequest;
	PGconn *conn;
		
	pthread_t thread_id;
	void threadFunction(void* args);
	
	struct threadArgsStruct
	{
		char *cConnectionString;
		cqueue<datatag> *cqWriteRequest;	
	} thargs;

public:
	void SetConnectionString(string sConnectionString);	
	void SetQueueWriteRequest(cqueue<datatag> *cqWriteRequest);
	static void *ThreadWrapper(void *thisPtr)
	{
		((sqlmanager_writes_thread*) thisPtr)->threadFunction(thisPtr);
		return NULL;
	}
	
	void ThreadStart();
	void ThreadJoin();
};

#endif // SQLMANAGER_WRITES_THREAD_H
