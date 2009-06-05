//
// C++ Interface: game
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef GAME_H
#define GAME_H

#include <taskengine/taskengine.h>

class GameImpl;

class Game : public Task
{
	public:
		Game();
		~Game();
	protected:
		bool doStep();
		void threadWillStart();
		void threadWillStop();
	private:
		boost::shared_ptr<GameImpl> impl;
};


#endif