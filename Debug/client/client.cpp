#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
using namespace std;

#define UNIX_DOMAIN "/tmp/mouse.sock"


enum Motion {
		/*init*/
		Idle = 0,
		/*basic search motions*/
		Start = 1, Stop = 2, Foward = 3, TurnLeft = 4, TurnRight = 5, TurnAround = 6, AroundLeft = 7, AroundRight = 8,
		/*rush motions*/
		RushStart = 9, RushStop = 10, cRushFoward = 11, cTurnLeft90 = 12, cTurnRight90 = 13,  cTurnLeft180 = 14, cTurnRight180 = 15,
		inTurnLeft45 = 16, outTurnLeft45 = 17, inTurnRight45 = 18, outTurnRight45 = 19,
		inTurnLeft135 = 20, outTurnLeft135 = 21, inTurnRight135 = 22, outTurnRight135 = 23,
		vTurnLeft90 =24, vTurnRight90=25, tRushFowrad = 26,
		Busy = 255
	};

int main()
{
	int sockfd;
	char motion = 1, motions[255];
	char status = 0;//wall status
	struct sockaddr_un servaddr;
	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	memset(&servaddr, sizeof(servaddr), 0);

	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIX_DOMAIN);

	connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	cout << "success connected" << endl;

	/*start*/
	motion = 1;
	write(sockfd, &motion, 1);

	int choose;
	while(1)
	{
		read(sockfd, &status, 1);
		cout << (int)status << endl;
		memset(motions, 0, sizeof(motions));
		switch(status)
		{
		case 0x07:
			motions[0] = Stop;
			motions[1] = TurnAround;
			motions[2] = Start;
			write(sockfd, motions, 3);
			break;
		case 0x05:
			motion = TurnLeft;
			write(sockfd, &motion, 1);
			break;
		case 0x03:
			motion = TurnRight;
			write(sockfd, &motion, 1);
			break;
		case 0x06:
			motion = Foward;
			write(sockfd, &motion, 1);
			break;
		case 0x04:
			srand(time(NULL));
			choose = rand() % 2;
			motion = choose == 0 ? Foward : TurnLeft;
			write(sockfd, &motion, 1);
			break;
		case 0x02:
			srand(time(NULL));
			choose = rand() % 2;
			motion = choose == 0 ? Foward : TurnRight;
			write(sockfd, &motion, 1);
			break;
		case 0x01:
			srand(time(NULL));
			choose = rand() % 2;
			motion = choose == 0 ? TurnLeft : TurnRight;
			write(sockfd, &motion, 1);
			break;
		case 0x00:
			srand(time(NULL));
			choose = rand() % 3;
			motion = choose == 0 ? TurnLeft : 
					 choose == 1 ? TurnRight:
					 Foward;
			write(sockfd, &motion, 1);
			break;
		default:
			break;
		}
	}

	close(sockfd);

	return 0;
}
