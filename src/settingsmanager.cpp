//
// C++ Implementation: settingsmanager
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "settingsmanager.h"
#include "Ogre.h"

DataContainer SettingsManager::getSetting(const std::string& name)
{
	return this->settings[name];
}

void SettingsManager::addSetting(const std::string& name, DataContainer set)
{
	this->settings[name] = set;
}