//
// C++ Interface: resourcemanager
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <taskengine/taskengine.h>
#include "singleton.h"
#include <map>

class ResourceManager : public Singleton<ResourceManager>
{
	public:
		DataContainer loadResource(const std::string& name);
	private:
		std::map<std::string, DataContainer> resources;
};

#endif
