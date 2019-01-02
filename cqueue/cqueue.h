#ifndef CQUEUE_H
#define CQUEUE_H

#include <queue>
#include <pthread.h>
#include <iostream>

using namespace std;

template <typename T> 
class cqueue
{
private:
	queue<T> _queue_;
	pthread_mutex_t mutex;
	pthread_cond_t cond;

public:
	cqueue()
	{
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
	}

	void push(T &data)
	{
		pthread_mutex_lock(&mutex);
		_queue_.push(data);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	
	int size()
	{
		int iSize;
		pthread_mutex_lock(&mutex);
		iSize = _queue_.size();
		pthread_mutex_unlock(&mutex);		
		
		return iSize;
	}

	void pop(T &popped_value)
	{
		pthread_mutex_lock(&mutex);
		while(_queue_.empty())
			pthread_cond_wait(&cond, &mutex);
		popped_value = _queue_.front();
		_queue_.pop();
		pthread_mutex_unlock(&mutex);
	}
};

#endif // CQUEUE_H
