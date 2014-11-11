/*
 * Mouse.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */
#include "Mouse.h"

Mouse::Mouse(SceneManager* sceneManager, int x, int y) :
mSceneManager(sceneManager),
mMouse(0), mMouseNode(0),
mCurDirect(North), mDirect(Vector3::UNIT_X),mMotionOver(true),
mWallLength(18.0f), mVelocity(0.0f), mAcc(2.0f), mMaxSpeed(0.6f),
mDirection(Vector3::UNIT_Z), remainAngle(90)
{
	createMouse(x, y);
//	mMotions.push(Start);
	for(int i = 0; i < 81; i++)
	mMotions.push(TurnRight);
//	mMotions.push(Foward);
//	mMotions.push(Foward);
//	mMotions.push(Stop);
//	mMotions.push(TurnAround);
//	mMotions.push(TurnAround);
//	mMotions.push(TurnAround);
//	mMotions.push(TurnAround);
	cout << "length of motions" << mMotions.size() << endl;
}

Mouse::~Mouse()
{

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

}

void Mouse::frameRenderingQueued(const FrameEvent& evt)
{
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
	}
	else
	{
			mVelocity -= mAcc * evt.timeSinceLastFrame;
			if(mVelocity <0) mVelocity = 0;
	 }

	float deltaX = mVelocity * evt.timeSinceLastFrame * 100;
	cout << "distance:" << distant << "deltax" << deltaX << endl;
	if (distant > deltaX)
		distant -= deltaX;
	else
	{
		deltaX = distant;
		if(start) mVelocity = mMaxSpeed;
		else mVelocity = 0;
		mMotionOver = true;
		distant = 9;
		cout << "motion over.................................." << endl;
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
		cout << "north" << endl;
		break;
	case East:
		deltaX = radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		deltaY = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaY *= -1;
		cout << "east" << endl;
		break;
	case South:
		deltaX = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaX *= -1;
		deltaY = -radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		cout << "south" << endl;
		break;
	case West:
		deltaX = -radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		deltaY = radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaY *= -1;
		cout << "west" << endl;
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
