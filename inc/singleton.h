/***************************************************************************
 *   Copyright (C) 2009 by Mononofu   *
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

#ifndef SINGLETON_H
#define SINGLETON_H

#include "destroyer.h"

template<typename T> class Singleton
{
	public:
		static T& Instance() 
		{
			if(instance == 0)
			{
				instance = new T;
				myDestroyer.SetDoomed ( instance );
			}
			return *instance;
		}
		static T* InstancePtr() 
		{
			if(instance == 0)
			{
				instance = new T;
				myDestroyer.SetDoomed ( instance );
			}
			assert(instance);
			return instance; 
		}
	protected:
		friend class Destroyer<T>;
		Singleton() {}
		~Singleton() {}
		static T* instance;
		static Destroyer<T> myDestroyer;
};

template <typename T> T* Singleton<T>::instance = 0;
template <typename T> Destroyer<T> Singleton<T>::myDestroyer;

#endif