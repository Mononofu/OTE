//
// C++ Interface: objectregistry
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OBJECTREGISTRY_H
#define OBJECTREGISTRY_H

#include "singleton.h"
#include <string>
#include <map>

class ObjectRegistry : public Singleton<ObjectRegistry>
{
	public:
		int addObject(const std::string& name);
		const std::string& getNameForID(int id);
		ObjectRegistry();
		~ObjectRegistry() {}
	private:
		int objectIDCounter;
		std::map<int, std::string> objects;

};

#endif
