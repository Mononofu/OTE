//
// C++ Interface: input
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef INPUT_H
#define INPUT_H

#include <taskengine/taskengine.h>

class InputImpl;

class Input : public Task
{
	public:
		Input();
		~Input();
	protected:
		bool doStep();
		void threadWillStart();
		void threadWillStop();
	private:
		boost::shared_ptr<InputImpl> impl;
};

#endif
