#ifndef _THREADBASE_H_
#define _THREADBASE_H_

#include <pthread.h>
typedef void *(*ThreadCall)(void *arg);

class ThreadBase
{
public:
	ThreadBase(bool aDetached = false);
	virtual ~ThreadBase();

	bool threadStart();
	bool threadStop();

	pthread_t getId() const;
	int ErrorCode() const;

protected:
	virtual void Run();

private:
	pthread_t mId;
	int mErrorCode;
	bool mDetached;

	static void *threadFunc(void *arg);
};





#endif
