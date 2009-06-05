//
// C++ Implementation: resourcemanager
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "resourcemanager.h"
#include "Ogre.h"


DataContainer ResourceManager::loadResource(const std::string& name)
{
	if(resources.find(name) == resources.end())
	{
		if(name.find("mesh") != std::string::npos)
		{
			std::pair<Ogre::ResourcePtr, bool> pair = Ogre::MeshManager::getSingleton().createOrRetrieve(name, "General");
			Ogre::MeshPtr mesh = pair.first;
			if(! pair.second)
			{
				Derr << "Ressource " << name << " couldn't be loaded";
			}
			resources[name] = DataContainer(mesh);
		}
	}
	return DataContainer(resources[name]);
}