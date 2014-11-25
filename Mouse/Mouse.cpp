/*
 * 
 * Mouse.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */
#include "Mouse.h"

Mouse::Mouse(SceneManager* sceneManager, Camera* camera,int x, int y) :
mSceneManager(sceneManager),mCamera(camera),
mMouse(0), mMouseNode(0),
mCurDirect(North), mDirect(Vector3::UNIT_X), mCurMotion(Idle),
mMotionOver(true), mCalibrated(false),
mWallLength(18.0f), mVelocity(0.0f), mAcc(2.0f), mMaxSpeed(0.6f),
mDirection(Vector3::UNIT_Z), remainAngle(90),
mFowardDistance(0), mLeftDistance(0), mRightDistance(0),
mWallStatus(0), mSimCom(0)
{
	mCurPos.x = -135;
	mCurPos.y = -5;
	mCurPos.z = -135;

	/*init mouse and com thread*/
	createMouse(x, y);
	mWallStatus = new queue<char>;
	mSimCom = new Com(mWallStatus);
	mSimCom->threadStart();

//	mCurMotion = Start;
}

Mouse::~Mouse()
{
	mSceneManager->destroyQuery(mFowardRayQuery);
	mSceneManager->destroyQuery(mLeftRayQuery);
	mSceneManager->destroyQuery(mRightRayQuery);
}

void Mouse::createMouse(int x, int y)
{
	mMouse = mSceneManager->createEntity("mouse", "razor.mesh");
	mMouseNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Vector3(0,0,0));
	mMouseNode->attachObject(mMouse);
	mMouseNode->setScale(0.05, 0.05, 0.05);
	mMouseNode->setPosition(Vector3(-135.0f + y*18.0f, -5, -135.0f + x * 18.0f ));
	mMouseNode->rotate(Vector3::UNIT_Y, Degree(90), Node::TS_WORLD);

	// create a particle system with 200 quota, then set its material and dimensions
	ParticleSystem* thrusters = mSceneManager->createParticleSystem(5);
	thrusters->setMaterialName("Examples/Flare");
	thrusters->setDefaultDimensions(5, 5);

	// create two emitters for our thruster particle system
	for (unsigned int i = 0; i < 2; i++)
	{
		ParticleEmitter* emitter = thrusters->addEmitter("Point");  // add a point emitter

		// set the emitter properties
		emitter->setAngle(Degree(3));
		emitter->setTimeToLive(0.1);
		emitter->setEmissionRate(25);
		emitter->setParticleVelocity(15);
		emitter->setDirection(Vector3::NEGATIVE_UNIT_Z);
		emitter->setColour(ColourValue::White, ColourValue::Red);
		emitter->setPosition(Vector3(i == 0 ? 5.7 : -18, 0, 0));
	}

	// attach our thruster particles to the rear of the ship
	SceneNode* thrustersNode = mMouseNode->createChildSceneNode(Vector3(0, 10, -70));
	thrustersNode->attachObject(thrusters);

	// initial ray to simulation infrared
	mFowardRayQuery = mSceneManager->createRayQuery(mFowardRay);
	mFowardRayQuery->setSortByDistance(true);
	mLeftRayQuery = mSceneManager->createRayQuery(mLeftRay);
	mLeftRayQuery->setSortByDistance(true);
	mRightRayQuery = mSceneManager->createRayQuery(mRightRay);
	mRightRayQuery->setSortByDistance(true);
}

void Mouse::calibration()
{
	int deltax, deltay;
	Vector3 deltaPos(1, 1, 1);

	if(mCalibrated == false)
	{
		switch(mCurMotion)
		{
		case Start:
		case Stop:
		case Foward:
			switch(mCurDirect)
			{
			case North:
				deltaPos *= Vector3::UNIT_X;
				break;
			case East:
				deltaPos *= Vector3::UNIT_Z;
				break;
			case South:
				deltaPos *= Vector3::NEGATIVE_UNIT_X;
				break;
			case West:
				deltaPos *= Vector3::NEGATIVE_UNIT_Z;
				break;
			}
			if(mCurMotion == Foward)
				deltaPos*=18;
			else
				deltaPos*=9;
			mCurPos += deltaPos;
			mMouseNode->setPosition(mCurPos);
			break;
		case TurnLeft:
		case TurnRight:
			switch(mCurDirect)
			{
			case North:
				deltax = mCurMotion == TurnLeft ? 9 : -9;
				deltay = 9;
				break;
			case East:
				deltax =9;
				deltay = mCurMotion == TurnLeft ? -9 : 9;
				break;
			case South:
				deltax = mCurMotion == TurnLeft ? -9 : 9;
				deltay = -9;
				break;
			case West:
				deltax =-9;
				deltay = mCurMotion == TurnLeft ? 9 : -9;
				break;
			}
			mCurPos += Vector3(deltay, 0, deltax);
			mMouseNode->setPosition(mCurPos);
			break;
		default:
			break;
		}

		mCalibrated = true; // calibrated
	}
}

void Mouse::frameRenderingQueued(const FrameEvent& evt)
{
	if(mSimCom->getSimStart())
	{
		Motion motion;
		while(  (motion = static_cast<Motion>(mSimCom->getNextMotion()) ) != 0 )
		{
			mMotions.push(motion);
		}

		/*action motion*/
		if(mMotionOver == true) //last motion complete
		{
			calibration(); // calibration position of mouse
			if (!mMotions.empty())
			{
				mCurMotion = mMotions.front(); //pick up next motion
				mMotions.pop();
				//cout << "main  :  new motion:" << mCurMotion << endl;
				mMotionOver = mCalibrated = false;
			}
			else if (mMotions.empty())
			{
				mWallStatus->push(lookup());
			}
		}

		if(mMotionOver == false) //mouse is acting a motion
		{
			switch(mCurMotion)
			{
			case Start:
				start(evt);
				break;
			case Foward:
				foward(evt);
				break;
			case TurnLeft:
				turn(evt, true);
				break;
			case TurnRight:
				turn(evt, false);
				break;
			case TurnAround:
				turnAround(evt);
				break;
			case Stop:
				start(evt, false);
				break;
			default:
				break;
			}
		}
	}//sim start
}

char Mouse::lookup()
{
	Vector3 rayPos;
	char status = 0;
	/*detect the wall*/
	switch(mCurDirect) //set ray source and direct
	{
	case North:
		rayPos = Vector3(3, 0, 0) + mMouseNode->_getDerivedPosition();
		mFowardRay.setOrigin(rayPos);
		rayPos = Vector3(3, 0, -3) + mMouseNode->_getDerivedPosition();
		mLeftRay.setOrigin(rayPos);
		rayPos = Vector3(3, 0, 3) + mMouseNode->_getDerivedPosition();
		mRightRay.setOrigin(rayPos);
		mFowardRay.setDirection(Vector3::UNIT_X);
		mLeftRay.setDirection(Vector3::NEGATIVE_UNIT_Z);
		mRightRay.setDirection(Vector3::UNIT_Z);
		break;
	case East:
		rayPos = Vector3(0, 0, 3) + mMouseNode->_getDerivedPosition();
		mFowardRay.setOrigin(rayPos);
		rayPos = Vector3(3, 0, 3) + mMouseNode->_getDerivedPosition();
		mLeftRay.setOrigin(rayPos);
		rayPos = Vector3(-3, 0, 3) + mMouseNode->_getDerivedPosition();
		mRightRay.setOrigin(rayPos);
		mFowardRay.setDirection(Vector3::UNIT_Z);
		mLeftRay.setDirection(Vector3::UNIT_X);
		mRightRay.setDirection(Vector3::NEGATIVE_UNIT_X);
		break;
	case South:
		rayPos = Vector3(-3, 0, 0) + mMouseNode->_getDerivedPosition();
		mFowardRay.setOrigin(rayPos);
		rayPos = Vector3(-3, 0, 3) + mMouseNode->_getDerivedPosition();
		mLeftRay.setOrigin(rayPos);
		rayPos = Vector3(-3, 0, -3) + mMouseNode->_getDerivedPosition();
		mRightRay.setOrigin(rayPos);
		mFowardRay.setDirection(Vector3::NEGATIVE_UNIT_X);
		mLeftRay.setDirection(Vector3::UNIT_Z);
		mRightRay.setDirection(Vector3::NEGATIVE_UNIT_Z);
		break;
	case West:
		rayPos = Vector3(0, 0, -3) + mMouseNode->_getDerivedPosition();
		mFowardRay.setOrigin(rayPos);
		rayPos = Vector3(-3, 0, -3) + mMouseNode->_getDerivedPosition();
		mLeftRay.setOrigin(rayPos);
		rayPos = Vector3(3, 0, -3) + mMouseNode->_getDerivedPosition();
		mRightRay.setOrigin(rayPos);
		mFowardRay.setDirection(Vector3::NEGATIVE_UNIT_Z);
		mLeftRay.setDirection(Vector3::NEGATIVE_UNIT_X);
		mRightRay.setDirection(Vector3::UNIT_X);
		break;
	}

	mFowardRayQuery->setRay(mFowardRay);
	Ogre::RaySceneQueryResult &fowardQuery = mFowardRayQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = fowardQuery.begin();
	for (; itr != fowardQuery.end(); itr++)
	{
		if(itr->movable && itr->movable->getName() != ""
						&& itr->movable->getName() != "mouse"
						&& itr->movable->getName() != mCamera->getName())
		{
			mFowardDistance = itr->distance;
			break;
		}
	}
	mLeftRayQuery->setRay(mLeftRay);
	Ogre::RaySceneQueryResult &leftQuery = mLeftRayQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr1 = leftQuery.begin();
	for (; itr1 != leftQuery.end(); itr1++)
	{
		if(itr1->movable && itr1->movable->getName() != ""
						&& itr1->movable->getName() != "mouse"
						&& itr1->movable->getName() != mCamera->getName())
		{
			mLeftDistance = itr1->distance;
			break;
		}
	}
	mRightRayQuery->setRay(mRightRay);
	Ogre::RaySceneQueryResult &rightQuery = mRightRayQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr3 = rightQuery.begin();
	for (; itr3 != rightQuery.end(); itr3++)
	{
		if(itr3->movable && itr3->movable->getName() != ""
						&& itr3->movable->getName() != "mouse"
						&& itr3->movable->getName() != mCamera->getName())
		{
			mRightDistance = itr3->distance;
			break;
		}
	}

	if(mFowardDistance < 20.0f)
		status |= 0x1;
	if(mLeftDistance < 10.0f)
		status |= 0x2;
	if(mRightDistance < 10.0f)
		status |= 0x4;

	return status;
}

void Mouse::start(const FrameEvent& evt, bool start)
{
	static float distant = 9;

	if(start)
	{
			mVelocity += mAcc * evt.timeSinceLastFrame;
			if(mVelocity > mMaxSpeed) mVelocity = mMaxSpeed;
	}
	else
	{
			mVelocity -= mAcc * evt.timeSinceLastFrame;
			if(mVelocity <0) mVelocity = 0;
	 }

	float deltaX = mVelocity * evt.timeSinceLastFrame * 100;
	if (distant > deltaX)
	{
		if(deltaX == 0)
		{
			deltaX = distant;
			mMotionOver = true;
			distant = 9;
			cout << "mouse stopped..." << endl;
		}
		distant -= deltaX;
	}
	else
	{
		deltaX = distant;
		if(start) mVelocity = mMaxSpeed;
		else mVelocity = 0;
		mMotionOver = true;
		distant = 9;
		cout << "mouse started..." << endl;
	}
	mMouseNode->translate( mDirect * deltaX, Node::TS_WORLD);
}

void Mouse::foward(const FrameEvent& evt)
{
	Real move;
	static Real length = mWallLength;
	move = evt.timeSinceLastFrame * mVelocity * 100;
	if(length > move) length -= move;
	else {
		move = length;
		mMotionOver = true;
		length = mWallLength;
	}
	mMouseNode->translate( mDirect * move, Node::TS_WORLD);
}

void Mouse::turn(const FrameEvent& evt, bool turnLeft)
{
	static float W1=0, W2 = 0;
	float turnTime = 0.2356;
	float radius = mWallLength/2;
	float deltaX, deltaY;
	float deltaW = evt.timeSinceLastFrame / turnTime * 90;

	if(remainAngle > deltaW)
	{
		remainAngle -= deltaW;
	}
	else {
		deltaW = remainAngle;
		mMotionOver = true;
	}

	W2 += deltaW;
	switch(mCurDirect)
	{
	case North:
		deltaX = radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2))) ;
		if(turnLeft) deltaX *= -1;
		deltaY = radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		break;
	case East:
		deltaX = radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		deltaY = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaY *= -1;
		break;
	case South:
		deltaX = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaX *= -1;
		deltaY = -radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		break;
	case West:
		deltaX = -radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		deltaY = radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaY *= -1;
		break;
	}
	W1 = W2; // save last W

	if(turnLeft) deltaW *= -1;
	mMouseNode->rotate(Vector3::UNIT_Y, Degree(-deltaW));
	mMouseNode->translate(Vector3(deltaY, 0, deltaX), Node::TS_WORLD);

	if(mMotionOver)
	{
		switch(mCurDirect)
		{
		case North:
			mCurDirect = turnLeft ? West : East;
			mDirect = turnLeft ? Vector3::NEGATIVE_UNIT_Z : Vector3::UNIT_Z;
			break;
		case East:
			mCurDirect = turnLeft ? North : South;
			mDirect = turnLeft ? Vector3::UNIT_X : Vector3::NEGATIVE_UNIT_X;
			break;
		case South:
			mCurDirect = turnLeft ? East : West;
			mDirect = turnLeft ? Vector3::UNIT_Z : Vector3::NEGATIVE_UNIT_Z;
			break;
		case West:
			mCurDirect = turnLeft ? South : North;
			mDirect = turnLeft ? Vector3::NEGATIVE_UNIT_X : Vector3::UNIT_X;
			break;
		}
		remainAngle = 90;
		W1 =W2 = 0;
	}

}

void Mouse::turnAround(const FrameEvent& evt)
{
	const static float turnTime = 0.2357;
	static float angle = 180.0f;
	float deltaW = evt.timeSinceLastFrame / turnTime * 180;

	if(angle > deltaW)
		angle -= deltaW;
	else
	{
		deltaW = angle;
		mMotionOver = true;
	}

	mMouseNode->rotate(Vector3::UNIT_Y, Degree(deltaW), Node::TS_WORLD);

	if(mMotionOver)
	{
		switch(mCurDirect)
		{
		case North:
			mCurDirect = South;
			mDirect =  Vector3::NEGATIVE_UNIT_X;
			break;
		case East:
			mCurDirect = West;
			mDirect = Vector3::NEGATIVE_UNIT_Z;
			break;
		case South:
			mCurDirect = North;
			mDirect = Vector3::UNIT_X;
			break;
		case West:
			mCurDirect = East;
			mDirect = Vector3::UNIT_Z;
			break;
		}
		angle = 180.0f;
	}
}

//		switch(status)
//		{
//		case 0x07:
//			mMotions.push(Stop);
//			mMotions.push(TurnAround);
//			mMotions.push(Start);
//			break;
//		case 0x05:
//			mMotions.push(TurnLeft);
//			break;
//		case 0x03:
//			mMotions.push(TurnRight);
//			break;
//		case 0x06:
//			mMotions.push(Foward);
//			break;
//		case 0x04:
//			mMotions.push(Foward);
//			break;
//		case 0x02:
//			mMotions.push(Foward);
//			break;
//		case 0x01:
//			mMotions.push(TurnLeft);
//			break;
//		case 0x0:
//			mMotions.push(Foward);
//			break;
//		default:
//			break;
//		}
