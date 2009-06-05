/***************************************************************************
 *   Copyright (C) 2008 by Mononofu,,,   *
 *   mononofu@mononofu-laptop   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef LISTENER_H
#define LISTENER_H

#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include <OgreNewt.h>

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

class MyFrameListener : public Ogre::FrameListener, public Ogre::WindowEventListener
{
	public:
// Constructor takes a RenderWindow because it uses that to determine input context
		MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, bool bufferedKeys = false, bool bufferedMouse = false,
			bool bufferedJoy = false ) :
			mCamera(cam), mTranslateVector(Ogre::Vector3::ZERO), mWindow(win), mStatsOn(true), mNumScreenShots(0),
				mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(Ogre::TFO_BILINEAR),
					   mAniso(1), mSceneDetailIndex(0), mMoveSpeed(100), mRotateSpeed(36), mDebugOverlay(0)
					   {
						   using namespace OIS;

						   mDebugOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	//Set initial mouse clipping size
						   windowResized(mWindow);

						   showDebugOverlay(true);
					   }
							
					   virtual ~MyFrameListener()
					   {
					   }

					   void moveCamera();
					   void showDebugOverlay(bool show);
					   bool frameStarted(const Ogre::FrameEvent& evt);
					   bool frameEnded(const Ogre::FrameEvent& evt);

	protected:
		void updateStats();

		Ogre::Camera* mCamera;

		Ogre::Vector3 mTranslateVector;
		Ogre::RenderWindow* mWindow;
		bool mStatsOn;

		std::string mDebugText;

		unsigned int mNumScreenShots;
		float mMoveScale;
		Ogre::Degree mRotScale;
// just to stop toggles flipping too fast
		Ogre::Real mTimeUntilNextToggle ;
		Ogre::Radian mRotX, mRotY;
		Ogre::TextureFilterOptions mFiltering;
		int mAniso;

		int mSceneDetailIndex ;
		Ogre::Real mMoveSpeed;
		Ogre::Degree mRotateSpeed;
		Ogre::Overlay* mDebugOverlay;
};


#endif
