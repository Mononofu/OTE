//
// C++ Interface: physics
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef PHYSICS_H
#define PHYSICS_H

#include <taskengine/taskengine.h>

class PhysicsImpl;

class Physics : public Task
{
	public:
		Physics();
		~Physics();
	protected:
		bool doStep();
		void threadWillStart();
		void threadWillStop();
	private:
		boost::shared_ptr<PhysicsImpl> impl;
};


#endif
