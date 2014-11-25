/*
 * Com.h
 *
 *  Created on: Nov 21, 2014
 *      Author: GaryTao
 */

#ifndef COM_H_
#define COM_H_

#define _LINIUX_VERSION_

#include "Thread.h"
#include <queue>
using namespace std;

/* Linux version Com*/
#ifdef _LINIUX_VERSION_
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define UNIX_DOMAIN "/tmp/mouse.sock"

#else /*WIN_32*/

#endif


class Com : public ThreadBase
{
public:
	enum Motion {
		/*basic search motions*/
		Start = 1, Stop = 2, Foward = 3, TurnLeft = 4, TurnRight = 5, TurnAround = 6, AroundLeft = 7, AroundRight = 8,
		/*rush motions*/
		RushStart = 9, RushStop = 10, cRushFoward = 11, cTurnLeft90 = 12, cTurnRight90 = 13,  cTurnLeft180 = 14, cTurnRight180 = 15,
		inTurnLeft45 = 16, outTurnLeft45 = 17, inTurnRight45 = 18, outTurnRight45 = 19,
		inTurnLeft135 = 20, outTurnLeft135 = 21, inTurnRight135 = 22, outTurnRight135 = 23,
		vTurnLeft90 =24, vTurnRight90=25, tRushFowrad = 26
	};

public:
	Com(queue<char> *wallStat);
	~Com();
	bool initiServer();
	int getNextMotion();
	int getConnfd() const;
	bool getSimStart() const;

private:
	virtual void Run();

	/*motion*/
	queue<char>* mWallStat;
	queue<Motion> mNextMotion;
	char motions[255];

	/*unix domain members*/
	sockaddr_un servaddr, cliaddr;
	socklen_t clilen;
	int listenfd, connfd;
	bool simStart;
};


#endif /* COM_H_ */
