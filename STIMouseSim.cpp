/*
 * STIMouseSim.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */
#include "STIMouseSim.h"

STIMouseSim::STIMouseSim():
mMouse(0), mMaze(0),mMouseStart(false),mMouseStatePanel(0)
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
	mMouse = new Mouse(mSceneMgr);
}

void STIMouseSim::createFrameListener(void)
{
	BaseApplication::createFrameListener();
	Ogre::StringVector items;
	items.push_back("mouse.X");
	items.push_back("mouse.Y");
	items.push_back("mouse.speed");
	mMouseStatePanel = mTrayMgr->createParamsPanel(TL_BOTTOMRIGHT, "MouseState", 200, items);
}

bool STIMouseSim::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mMouseStart)
		mMouse->frameRenderingQueued(evt);

	mMouseStatePanel->setParamValue(0, Ogre::StringConverter::toString(mMouse->mMouseNode->_getDerivedPosition().z + 135));
	mMouseStatePanel->setParamValue(1, Ogre::StringConverter::toString(mMouse->mMouseNode->_getDerivedPosition().x + 135));

	return BaseApplication::frameRenderingQueued(evt);
}

// OIS::KeyListener
bool STIMouseSim::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_SPACE)
	{
		mMouseStart = true;
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
	return BaseApplication::mousePressed(arg, id);
}
bool STIMouseSim::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
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


