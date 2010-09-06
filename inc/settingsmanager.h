//
// C++ Interface: settingsmanager
//
// Description: 
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <taskengine/taskengine.h>
#include "singleton.h"
#include <map>

class SettingsManager : public Singleton<SettingsManager>
{
	public:
		DataContainer getSetting(const std::string& name);
		void addSetting(const std::string& name, DataContainer set);
	private:
		std::map<std::string, DataContainer> settings;
};

#endif
