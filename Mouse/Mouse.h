/*
 * Mouse.h
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */

#ifndef MOUSE_H_
#define MOUSE_H_

#include "../BaseApplication.h"
using namespace Ogre;
using namespace std;

class Mouse
{
public:
	Mouse(SceneManager* sceneManager, int x = 0, int y = 0);
	~Mouse();

	void start(const FrameEvent& evt, bool start = true);
	void foward(const FrameEvent& evt);
	void turn(const FrameEvent& evt, bool turnLeft = true);
	void turnAround(const FrameEvent& evt);
	void frameRenderingQueued(const Ogre::FrameEvent& evt);

//protected:
	enum CurrenDiretion{North, East, South, West};
	void createMouse(int x, int y);

	SceneManager* mSceneManager;
	Entity* mMouse;
	SceneNode* mMouseNode;
	CurrenDiretion mCurDirect;
	Vector3 mDirect;

	enum Motion{Start, Foward, TurnLeft, TurnRight, TurnAround, Stop};
	//motion queue
	queue<Motion> mMotions;
	Motion mCurMotion;
	bool mMotionOver;

private:
	//just for test to move
	Real mWallLength;
	Real mVelocity;
	Real mAcc;
	Real mMaxSpeed;
	Vector3 mDirection;
	Real remainAngle;
};




#endif /* MOUSE_H_ */
