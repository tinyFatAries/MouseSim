/*
 * Mouse.h
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */

#ifndef MOUSE_H_
#define MOUSE_H_

#include "../BaseApplication.h"
#include "../Com/Com.h"
using namespace Ogre;
using namespace std;

class Mouse
{
public:
	Mouse(SceneManager* sceneManager, Camera *camera, int x = 0, int y = 0);
	~Mouse();

	void calibration();
	char lookup();
	void start(const FrameEvent& evt, bool start = true);
	void foward(const FrameEvent& evt);
	void turn(const FrameEvent& evt, bool turnLeft = true);
	void turnAround(const FrameEvent& evt);
	void frameRenderingQueued(const Ogre::FrameEvent& evt);

//protected:
	enum CurrenDiretion{North, East, South, West};
	void createMouse(int x, int y);

	SceneManager* mSceneManager;
	Camera* mCamera;
	Entity* mMouse;
	SceneNode* mMouseNode;
	CurrenDiretion mCurDirect;
	Vector3 mDirect;
	Vector3 mCurPos;

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
	//motion queue
	queue<Motion> mMotions;
	Motion mCurMotion;
	bool mMotionOver, mCalibrated;

private:
	/*motion parameters*/
	Real mWallLength;
	Real mVelocity;
	Real mAcc;
	Real mMaxSpeed;
	Vector3 mDirection;
	Real remainAngle;

	/*wall detect variables*/
	Real mFowardDistance, mLeftDistance, mRightDistance;
	RaySceneQuery *mFowardRayQuery, *mLeftRayQuery, *mRightRayQuery;
	Ray mFowardRay, mLeftRay, mRightRay;

	/*wall status*/
	queue<char>* mWallStatus;
	Com *mSimCom;
};




#endif /* MOUSE_H_ */
