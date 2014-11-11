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
	mMotions.push(Start);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(TurnRight);
	mMotions.push(TurnLeft);
	mMotions.push(Stop);
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
	if(mMotionOver == true && mMotions.empty() == false)
	{
		mCurMotion = mMotions.front();
		mMotions.pop();
		mMotionOver = false;
	}

	if(mMotionOver == false)
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
	if(start)
	{
		if(mVelocity < mMaxSpeed)
		{
			mVelocity += mAcc * evt.timeSinceLastFrame;
			if(mVelocity > mMaxSpeed)
				mVelocity = mMaxSpeed;
		}
		else
			mMotionOver = true;
	}
	else
	{
		if(mVelocity>0)
		{
			mVelocity -= mAcc * evt.timeSinceLastFrame;
			if(mVelocity < 0)
				mVelocity = 0;
		}
		else
			mMotionOver = true;
	}

	mMouseNode->translate( mDirect * mVelocity * evt.timeSinceLastFrame * 100, Node::TS_WORLD);
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
	case Mouse::North:
		deltaX = radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2))) ;
		if(turnLeft) deltaX *= -1;
		deltaY = radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		cout << "north" << endl;
		break;
	case Mouse::East:
		deltaX = radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		deltaY = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaY *= -1;
		cout << "east" << endl;
		break;
	case Mouse::South:
		deltaX = -radius * (Math::Cos(Degree(W1)) - Math::Cos(Degree(W2)));
		if(turnLeft) deltaX *= -1;
		deltaY = -radius * (Math::Sin(Degree(W2)) - Math::Sin(Degree(W1)));
		cout << "south" << endl;
		break;
	case Mouse::West:
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
		case Mouse::North:
			mCurDirect = turnLeft ? Mouse::West : Mouse::East;
			mDirect = turnLeft ? Vector3::NEGATIVE_UNIT_Z : Vector3::UNIT_Z;
			break;
		case Mouse::East:
			mCurDirect = turnLeft ? Mouse::North : Mouse::South;
			mDirect = turnLeft ? Vector3::UNIT_X : Vector3::NEGATIVE_UNIT_X;
			break;
		case Mouse::South:
			mCurDirect = turnLeft ? Mouse::East : Mouse::West;
			mDirect = turnLeft ? Vector3::UNIT_Z : Vector3::NEGATIVE_UNIT_Z;
			break;
		case Mouse::West:
			mCurDirect = turnLeft ? Mouse::South : Mouse::North;
			mDirect = turnLeft ? Vector3::NEGATIVE_UNIT_X : Vector3::UNIT_X;
			break;
		}
		cout<< mDirect.x << "," << mDirect.y << "," << mDirect.z << endl;
		remainAngle = 90;
		W1 =W2 = 0;
	}

}

void Mouse::turnAround(const FrameEvent& evt)
{
	const static float turnMaxSpeed = 600.0f;
	const static float turnAcc = 2000.0f;
	static float turnSpeed = 0;
	static int turnStage = 0;


	if (turnStage == 0 && turnSpeed < turnMaxSpeed )
	{
		turnSpeed += evt.timeSinceLastFrame * turnAcc;
		mMouseNode->rotate(Vector3::UNIT_Y, Degree(turnSpeed), Node::TS_WORLD);
	}
	else
	{
		turnStage = 1;
	}

	if (turnStage == 1 && turnSpeed > 0)
	{
		turnSpeed -= evt.timeSinceLastFrame * turnAcc;
		mMouseNode->rotate(Vector3::UNIT_Y, Degree(turnSpeed), Node::TS_WORLD);
	}
	else
	{
		turnStage = 0;
		mMotionOver = true;
	}

}
