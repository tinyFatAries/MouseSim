/*
 * STIMouseSim.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */
#include "STIMouseSim.h"
#include <sys/time.h>

STIMouseSim::STIMouseSim():
mMicroMouse(0), mMaze(0),mMouseStart(false),mMouseStatePanel(0)
{

}

STIMouseSim::~STIMouseSim()
{

}

void STIMouseSim::createScene()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);
	mCamera->setPosition(-100, 300, -100);
	mCamera->lookAt(0 , 0, 0);

	/*build a Maze*/
	mMaze = new Maze(mSceneMgr);
	if(mMaze->createFromFile("tw2012", "txt") == false)
		cout << "error create maze" << endl;
#ifdef DEBUG
	mMaze->printMaze();
#endif
	/*setup a mouse*/
	mMicroMouse = new Mouse(mSceneMgr, mCamera);
}

void STIMouseSim::createFrameListener(void)
{
	BaseApplication::createFrameListener();
	Ogre::StringVector items;
	items.push_back("mouse.X");
	items.push_back("mouse.Y");
	items.push_back("foward");
	items.push_back("left");
	items.push_back("right");
	mMouseStatePanel = mTrayMgr->createParamsPanel(TL_BOTTOMRIGHT, "MouseState", 200, items);
//	mCheckBox = mTrayMgr->createCheckBox(TL_CENTER, "checkBox", "check box test", 100);
//	mTextBox = mTrayMgr->createTextBox(TL_CENTER, "textbox", "a textbox", 400,200);
//	mLabel = mTrayMgr->createLabel(TL_TOPLEFT, "label", "Mouse-Sim", 200);
//	mSlider = mTrayMgr->createThickSlider(TL_CENTER, "slider", "test for slider", 200, 50, 0, 200, 1);
//	mButton = mTrayMgr->createButton(TL_TOPRIGHT, "SimStart", "SimStart", 200);
//	mSelectMenu = mTrayMgr->createLongSelectMenu(TL_CENTER, "selectMenu", "select", 400, 200, 10, items);
	mTrayMgr->showBackdrop();
}

bool STIMouseSim::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
//	timeval ts, te;
//	gettimeofday(&ts, NULL);
	if(mMouseStart)
		mMicroMouse->frameRenderingQueued(evt);

	mMouseStatePanel->setParamValue(0, Ogre::StringConverter::toString(mMicroMouse->mMouseNode->_getDerivedPosition().z + 144));
	mMouseStatePanel->setParamValue(1, Ogre::StringConverter::toString(mMicroMouse->mMouseNode->_getDerivedPosition().x + 144));
//	mMouseStatePanel->setParamValue(2, Ogre::StringConverter::toString(mMicroMouse->mFowardDistance));
//	mMouseStatePanel->setParamValue(3, Ogre::StringConverter::toString(mMicroMouse->mLeftDistance));
//	mMouseStatePanel->setParamValue(4, Ogre::StringConverter::toString(mMicroMouse->mRightDistance));

	int ret = BaseApplication::frameRenderingQueued(evt);
//	gettimeofday(&te, NULL);
//	cout << te.tv_sec*1000000 + te.tv_usec - (ts.tv_sec*1000000 + ts.tv_usec) << "us" << endl;
	return ret;
}

// OIS::KeyListener
bool STIMouseSim::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_SPACE)
	{
		mMouseStart = !mMouseStart;
	}
	else if(arg.key == OIS::KC_Q)
	{
	}
	else if(arg.key == OIS::KC_E)
	{
	}
	return BaseApplication::keyPressed(arg);
}

bool STIMouseSim::keyReleased( const OIS::KeyEvent &arg )
{
	return BaseApplication::keyReleased(arg);
}

// OIS::MouseListener
bool STIMouseSim::mouseMoved( const OIS::MouseEvent &arg )
{
	return BaseApplication::mouseMoved(arg);
}

bool STIMouseSim::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseDown(arg, id)) return true;

	if(id == OIS::MB_Right) mTrayMgr->hideCursor(); //free look mode

	mMaze->injectMousePressed(arg, id, mTrayMgr, mSceneMgr, mCamera);

	return BaseApplication::mousePressed(arg, id);
}

void STIMouseSim::buttonHit(Button *btn)
{

}

bool STIMouseSim::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseUp(arg, id)) return true;
	mTrayMgr->showCursor();
	return BaseApplication::mouseReleased(arg, id);
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        STIMouseSim app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif


