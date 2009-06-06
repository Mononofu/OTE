//
// C++ Implementation: objectregistry
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "objectregistry.h"

ObjectRegistry::ObjectRegistry() : objectIDCounter(1)
{
}

int ObjectRegistry::addObject(const std::string& name)
{
	objects[++objectIDCounter] = name;
	return objectIDCounter;
}

const std::string& ObjectRegistry::getNameForID(int id)
{
	return objects[id];
}