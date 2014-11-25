/*
 * Com.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: GaryTao
 */

#include "Com.h"

Com::Com(queue<char> *wallStat) : ThreadBase(false), mWallStat(wallStat),
	clilen(0), listenfd(0), connfd(0) , simStart(false)
{

}

Com::~Com()
{

}

bool Com::initiServer()
{
	if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
		printf("error open socket!\n");
		return false;
	}

	unlink(UNIX_DOMAIN);
	memset(&servaddr, 0, sizeof(servaddr));
	strncpy(servaddr.sun_path, UNIX_DOMAIN, sizeof(servaddr.sun_path) - 1);
	servaddr.sun_family = AF_LOCAL;

	if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
		printf("bind error!\n");
		return false;
	}

	listen(listenfd, 1);

	return true;
}

void Com::Run()
{
	int n, i;
	char status;
	initiServer();
	printf("server start\n");
	/*start server*/
	while(1)	
	{
		clilen = sizeof(cliaddr);	

		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
			if(errno == EINTR)	
				continue;
			else {
				printf("accpet error %s\n", strerror(errno));	
				exit(1);
			}
		}

		/*start a simulate session*/
		while(1)
		{
			memset(motions, 0, sizeof(motions));
			n = read(connfd, &motions, sizeof(motions));
			if(n <0 && errno == EINTR)	
				continue;
			else if(n < 0) {
				printf("error closed by client\n");
				close(connfd);
				connfd = 0;
				simStart = false;
				break;
			}
			else if(n == 0) {
				printf("simulate completed\n");
				close(connfd);
				connfd = 0;
				simStart = false;
				break;
			}

			i = 0;
			while(motions[i]!=0) //get all the motions to queue
			{
				mNextMotion.push(static_cast<Motion>(motions[i]));
				i++;
			}

			if(simStart == false) simStart = true; // start a simulate

			while(mWallStat->empty());
			status = mWallStat->front();
			mWallStat->pop();

rewrite:
			n = write(connfd, &status, 1);
			if(n <0 && errno == EINTR)	
				goto rewrite;
			else if(n <0) {
				printf("error closed by client\n");
				close(connfd);
				connfd = 0;
				simStart = false;
				break;
			}
		}//end simulate session
	}
	close(listenfd);//server exit
}

int Com::getNextMotion() 
{
	int ret;
	if(!mNextMotion.empty())
	{
		ret = static_cast<int>(mNextMotion.front());
		mNextMotion.pop();
		return ret;
	}
		
	return 0;
}

int Com::getConnfd() const
{
	return connfd;
}

bool Com::getSimStart() const
{
	return simStart;
}
