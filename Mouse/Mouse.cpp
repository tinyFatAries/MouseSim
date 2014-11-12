/*
 * Mouse.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */
#include "Mouse.h"

Mouse::Mouse(SceneManager* sceneManager, Camera* camera,int x, int y) :
mSceneManager(sceneManager),mCamera(camera),
mMouse(0), mMouseNode(0),
mCurDirect(North), mDirect(Vector3::UNIT_X),mMotionOver(true),
mWallLength(18.0f), mVelocity(0.06f), mAcc(2.0f), mMaxSpeed(0.6f),
mDirection(Vector3::UNIT_Z), remainAngle(90),
mFowardDistance(0), mLeftDistance(0), mRightDistance(0)
{
	createMouse(x, y);
//	mMotions.push(Start);
//	for(int i = 0; i < 14; i++)
//		mMotions.push(Foward);
//	mMotions.push(TurnRight);
//	for(int i = 0; i < 14; i++)
//		mMotions.push(Foward);
//	mMotions.push(TurnRight);
//	for(int i = 0; i < 14; i++)
//		mMotions.push(Foward);
//	mMotions.push(TurnRight);
//	for(int i = 0; i < 14; i++)
//		mMotions.push(Foward);
//	mMotions.push(Stop);
//	mMotions.push(TurnAround);
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

void Mouse::frameRenderingQueued(const FrameEvent& evt)
{
	/*detect the wall*/
	switch(mCurDirect)
	{
	case North:
		mFowardRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(5, 0, 0));
		mLeftRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(5, 0, 0));
		mRightRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(5, 0, 0));
		mFowardRay.setDirection(Vector3::UNIT_X);
		mLeftRay.setDirection(Vector3::NEGATIVE_UNIT_Z);
		mRightRay.setDirection(Vector3::UNIT_Z);
		break;
	case East:
		mFowardRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, 5));
		mLeftRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, 5));
		mRightRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, 5));
		mFowardRay.setDirection(Vector3::UNIT_Z);
		mLeftRay.setDirection(Vector3::UNIT_X);
		mRightRay.setDirection(Vector3::NEGATIVE_UNIT_X);
		break;
	case South:
		mFowardRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(-5, 0, 0));
		mLeftRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(-5, 0, 0));
		mRightRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(-5, 0, 0));
		mFowardRay.setDirection(Vector3::NEGATIVE_UNIT_X);
		mLeftRay.setDirection(Vector3::UNIT_Z);
		mRightRay.setDirection(Vector3::NEGATIVE_UNIT_Z);
		break;
	case West:
		mFowardRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, -5));
		mLeftRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, -5));
		mRightRay.setOrigin(mMouseNode->_getDerivedPosition() + Vector3(0, 0, -5));
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
			//cout << "detected walls" << endl;
			Vector3 vec = itr->movable->getParentSceneNode()->_getDerivedPosition() - mMouseNode->_getDerivedPosition();
			mFowardDistance = vec.normalise();
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
			//cout << "detected walls" << endl;
			Vector3 vec = itr1->movable->getParentSceneNode()->_getDerivedPosition() - mMouseNode->_getDerivedPosition();
			mLeftDistance = vec.normalise();
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
			//cout << "detected walls" << endl;
			Vector3 vec = itr3->movable->getParentSceneNode()->_getDerivedPosition() - mMouseNode->_getDerivedPosition();
			mRightDistance = vec.normalise();
			break;
		}
	}


	/*action motion*/
	if(mMotionOver == true && mMotions.empty() == false) //mouse is idle
	{
		mCurMotion = mMotions.front();
		mMotions.pop();
		mMotionOver = false;
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
		cout<< mDirect.x << "," << mDirect.y << "," << mDirect.z << endl;
		remainAngle = 90;
		W1 =W2 = 0;
	}

}

void Mouse::turnAround(const FrameEvent& evt)
{;
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
