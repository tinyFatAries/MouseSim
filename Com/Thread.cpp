#include "Thread.h"
#include <iostream>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
using namespace std;
 
ThreadBase::ThreadBase(bool aDetached) 
	:mId(0), mErrorCode(0), mDetached(aDetached)
{
}

ThreadBase::~ThreadBase()
{
}

int ThreadBase::ErrorCode() const
{
	return mErrorCode;
}

pthread_t ThreadBase::getId() const
{
	return mId;
}

void* ThreadBase::threadFunc(void *arg)
{
	ThreadBase* self = static_cast<ThreadBase*>(arg);
	
	self->Run();

	pthread_exit(NULL);
}


bool ThreadBase::threadStart()
{
	if(mDetached)	
	{
		pthread_attr_t attr;
		if ((mErrorCode = pthread_attr_init(&attr)) != 0) {
			cout << "init pthread attr error, error code:" << mErrorCode << endl;
			return false;
		}
		
		if ((mErrorCode = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
			cout << "set attr detatchstate error, error code:" << mErrorCode << endl;
			pthread_attr_destroy(&attr);
			return false;
		}

		if ((mErrorCode = pthread_create(&mId, &attr, threadFunc, this)) != 0) {
			cout << "pthread create error, error code:" << mErrorCode << endl;
			return false;
		}

		pthread_attr_destroy(&attr);	
		cout << "tid:" << mId << endl;
	}
	else
	{
		if ((mErrorCode = pthread_create(&mId, NULL, threadFunc, this)) != 0) {
			cout << "pthread create error, error code:" << mErrorCode << endl;
			return false;
		}
		cout << "tid:" << mId << endl;
	}

	return true;
}

bool ThreadBase::threadStop()
{
	if ((mErrorCode = pthread_cancel(mId)) != 0) {
			cout << "pthread cancel error, error code:" << mErrorCode << endl;
			printf("error %s\n", strerror(mErrorCode));			
	}
	cout << "thread canceld" << endl;

	cout << "tid:" << mId << endl;
	if ((mErrorCode =  pthread_join(mId, NULL)) != 0) {
			cout << "pthread join error, error code:" << mErrorCode << endl;
			printf("error %s\n", strerror(mErrorCode));			
	}
	cout << "thread joined" << endl;

	return true;
}

void ThreadBase::Run()
{
}
