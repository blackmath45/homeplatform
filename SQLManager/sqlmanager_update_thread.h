#ifndef SQLMANAGER_UPDATE_THREAD_H
#define SQLMANAGER_UPDATE_THREAD_H

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

class sqlmanager_update_thread
{
private :
	string sConnectionString;
	cqueue<datatag> *cqReadChange;
	PGconn *conn;
	
	pthread_mutex_t *sqlmanager_update_thread_mutex;
	pthread_cond_t *sqlmanager_update_thread_condition;
		
	pthread_t thread_id;
	void threadFunction(void* args);
	
	struct threadArgsStruct
	{
		char *cConnectionString;
		cqueue<datatag> *cqReadChange;
		pthread_mutex_t *sqlmanager_update_thread_mutex;
		pthread_cond_t *sqlmanager_update_thread_condition;
	} thargs;

public:
	void SetConnectionString(string sConnectionString);	
	void SetQueueReadChange(cqueue<datatag> *cqReadChange);
	void SetThreadMutex(pthread_mutex_t *sqlmanager_update_thread_mutex);
	void SetThreadCondition(pthread_cond_t *sqlmanager_update_thread_condition);
	static void *ThreadWrapper(void *thisPtr)
	{
		((sqlmanager_update_thread*) thisPtr)->threadFunction(thisPtr);
		return NULL;
	}
	
	void ThreadStart();
	void ThreadJoin();
};

//TODO function STOP + JOIN + Passage pointeur queue

#endif // SQLMANAGER_UPDATE_THREAD_H
