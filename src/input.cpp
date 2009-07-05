//
// C++ Implementation: input
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "input.h"
#include "FeedDataTypes.h"

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

class InputImpl : public Task, public OIS::MouseListener, public OIS::KeyListener
{
	public:
		InputImpl() { }
		 // MouseListener
		bool mouseMoved(const OIS::MouseEvent &e);
		bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

   		 // KeyListener
		bool keyPressed(const OIS::KeyEvent &e);
		bool keyReleased(const OIS::KeyEvent &e);
		
		bool doStep();
		void threadWillStart();
		void threadWillStop();
	private:
		OIS::InputManager* mInputManager;
		OIS::Mouse*    mMouse;
		OIS::Keyboard* mKeyboard;
		InputMouseEvent lastMouseState;
};

Input::Input() : impl(new InputImpl) { }
Input::~Input() { }
bool Input::doStep() { return impl->step(); }
void Input::threadWillStart() { impl->threadWillStart(); }
void Input::threadWillStop() { impl->threadWillStop(); }

bool InputImpl::doStep()
{
#ifndef DEBUG_BUILD
	boost::this_thread::sleep(boost::posix_time::milliseconds(10));		// Wait 1/100 s
#endif
	if(mMouse)
		mMouse->capture();
	if(mKeyboard) 
		mKeyboard->capture();
	return running;
}

void InputImpl::threadWillStart()
{
	OIS::ParamList pl;
	DataContainer myHandle = InformationManager::Instance()->requestData("window.handle", 10);
	pl.insert(std::make_pair(std::string("WINDOW"), boost::any_cast<std::string>(myHandle.data) ));
	
	{
		//Setup OIS in non-exclusive mode
#if defined OIS_WIN32_PLATFORM 
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" ))); 
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE"))); 
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND"))); 
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE"))); 
#elif defined OIS_LINUX_PLATFORM 
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false"))); 
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false"))); 
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false"))); 
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true"))); 
#endif 
	}
	
	mInputManager = OIS::InputManager::createInputSystem( pl );

		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
	
	mMouse->setEventCallback(this);
	mMouse->hide(true);
	mKeyboard->setEventCallback(this);
}

void InputImpl::threadWillStop()
{
	mMouse->setEventCallback(NULL);
	mKeyboard->setEventCallback(NULL);
}

bool InputImpl::mouseMoved(const OIS::MouseEvent &e)
{
	lastMouseState.action = BUTTON_SAME;
	lastMouseState.mouseDeltaX = e.state.X.rel;
	lastMouseState.mouseDeltaY = e.state.Y.rel;
	lastMouseState.mouseX = e.state.X.abs;
	lastMouseState.mouseY = e.state.Y.abs;
	
	InformationManager::Instance()->postDataToFeed( "input_mouse", DataContainer(lastMouseState) );
	return true;
}
	
bool InputImpl::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	lastMouseState.action = BUTTON_PRESSED;
	
	if( id == OIS::MB_Left )
	{
		lastMouseState.type = BUTTON_MOUSE_LEFT;
	}
	else if( id == OIS::MB_Right )
	{
		lastMouseState.type = BUTTON_MOUSE_RIGHT;
	}
	else
	{
		lastMouseState.type = BUTTON_MOUSE_MIDDLE;
	}
	InformationManager::Instance()->postDataToFeed( "input_mouse", DataContainer(lastMouseState) );
	return true;
}
	
bool InputImpl::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	lastMouseState.action = BUTTON_RELEASED;
	
	if( id == OIS::MB_Left )
	{
		lastMouseState.type = BUTTON_MOUSE_LEFT;
	}
	else if( id == OIS::MB_Right )
	{
		lastMouseState.type = BUTTON_MOUSE_RIGHT;
	}
	else
	{
		lastMouseState.type = BUTTON_MOUSE_MIDDLE;
	}
	InformationManager::Instance()->postDataToFeed( "input_mouse", DataContainer(lastMouseState) );
	return true;
}


bool InputImpl::keyPressed(const OIS::KeyEvent &e)
{
	InputKeyboardEvent keyEv;
	keyEv.type = (input_keyboard_type) e.key;
	keyEv.action = BUTTON_PRESSED;	
	InformationManager::Instance()->postDataToFeed( "input_keyboard", DataContainer(keyEv) );
	return true;
}

bool InputImpl::keyReleased(const OIS::KeyEvent &e)
{
	InputKeyboardEvent keyEv;
	keyEv.type = (input_keyboard_type) e.key;
	keyEv.action = BUTTON_RELEASED;
	InformationManager::Instance()->postDataToFeed( "input_keyboard", DataContainer(keyEv) );
	return true;
}

