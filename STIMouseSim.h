/*
 * STIMouseSim.h
 *
 *  Created on: Nov 3, 2014
 *      Author: GaryTao
 */

#ifndef STIMOUSESIM_H_
#define STIMOUSESIM_H_

#include "BaseApplication.h"
#include "Mouse/Mouse.h"
#include "Maze/Maze.h"
using namespace Ogre;
using namespace OgreBites;

#define DEBUG

class STIMouseSim : public BaseApplication
{
public:
	STIMouseSim();
	virtual ~STIMouseSim();
protected:
	void createScene();
	void createFrameListener(void);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );
	// OIS::MouseListener
	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	// OgreBites::Listener
	void buttonHit(Button* btn );
private:
	Mouse* mMicroMouse;
	Maze* mMaze;
	bool mMouseStart;
	/*UI members*/
	OgreBites::ParamsPanel* mMouseStatePanel;
//	OgreBites::CheckBox* mCheckBox;
//	OgreBites::TextBox* mTextBox;
//	OgreBites::SelectMenu* mSelectMenu;
//	OgreBites::Label* mLabel;
//	OgreBites::Slider* mSlider;
//	OgreBites::Button* mButton;
};




#endif /* STIMOUSESIM_H_ */
